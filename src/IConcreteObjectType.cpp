//
//  IConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Cleanup.hpp"
#include "wisey/Composer.hpp"
#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FieldArrayOwnerVariable.hpp"
#include "wisey/FieldArrayReferenceVariable.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/HeapBuilder.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Names.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/ParameterReferenceVariableStatic.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

const unsigned int IConcreteObjectType::CONTROLLER_FIRST_LETTER_ASCII_CODE = 99u;
const unsigned int IConcreteObjectType::MODEL_FIRST_LETTER_ASCII_CODE = 109u;
const unsigned int IConcreteObjectType::NODE_FIRST_LETTER_ASCII_CODE = 110u;
const unsigned int IConcreteObjectType::THREAD_FIRST_LETTER_ASCII_CODE = 116u;

void IConcreteObjectType::declareTypeNameGlobal(IRGenerationContext& context,
                                                const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  string typeName = object->getTypeName();
  new GlobalVariable(*context.getModule(),
                     llvm::ArrayType::get(Type::getInt8Ty(llvmContext), typeName.length() + 1),
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     nullptr,
                     object->getObjectNameGlobalVariableName());
}

void IConcreteObjectType::defineTypeNameGlobal(IRGenerationContext& context,
                                               const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  string typeName = object->getTypeName();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, typeName);
  new GlobalVariable(*context.getModule(),
                     llvm::ArrayType::get(Type::getInt8Ty(llvmContext), typeName.length() + 1),
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     stringConstant,
                     object->getObjectNameGlobalVariableName());
}

Value* IConcreteObjectType::castTo(IRGenerationContext& context,
                                   const IConcreteObjectType* object,
                                   Value* fromValue,
                                   const IType* toType,
                                   int line) {
  if (toType == object) {
    return fromValue;
  }
  if (!object->canCastTo(context, toType)) {
    Cast::exitIncompatibleTypes(context, object, toType, line);
    return NULL;
  }
  LLVMContext& llvmContext = context.getLLVMContext();
  if (toType->isNative() && (toType->isReference() || toType->isPointer())) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 ICmpInst::ICMP_NE,
                                 fromValue,
                                 ConstantPointerNull::get(object->getLLVMType(context)),
                                 "");
  }
  
  const Interface* interface = (const Interface*) toType;
  llvm::PointerType* llvmType = interface->getLLVMType(context);
  int interfaceIndex = getInterfaceIndex(object, interface);
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = IRWriter::newBitCastInst(context, fromValue, int8Type->getPointerTo());
  Value* index[1];
  unsigned long thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  return IRWriter::newBitCastInst(context, thunk, llvmType);
}

int IConcreteObjectType::getInterfaceIndex(const IConcreteObjectType* object,
                                           const Interface* interface) {
  int index = 0;
  for (Interface* implementedInterface : object->getFlattenedInterfaceHierarchy()) {
    if (implementedInterface == interface) {
      return index;
    }
    index++;
  }
  return -1;
}

void IConcreteObjectType::initializeVTable(IRGenerationContext& context,
                                           const IConcreteObjectType* object,
                                           Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* vTableGlobal = context.getModule()->getNamedGlobal(object->getVTableName());
  
  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  Value* vTableStartCalculation = IRWriter::newBitCastInst(context, malloc, genericPointerType);

  for (unsigned int vTableIndex = 0; vTableIndex < getVTableSizeForObject(object); vTableIndex++) {
    Value* vTableStart;
    Value* index[1];
    unsigned int thunkBy = vTableIndex * Environment::getAddressSizeInBytes();
    index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
    vTableStart = IRWriter::createGetElementPtrInst(context, vTableStartCalculation, index);
    
    Value* vTablePointer =
    IRWriter::newBitCastInst(context, vTableStart, vTableType->getPointerTo());
    Value* idx[3];
    idx[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), vTableIndex);
    idx[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = IRWriter::createGetElementPtrInst(context, vTableGlobal, idx);
    BitCastInst* bitcast = IRWriter::newBitCastInst(context, initializerStart, vTableType);
    IRWriter::newStoreInst(context, bitcast, vTablePointer);
  }
}

void IConcreteObjectType::defineVTable(IRGenerationContext& context,
                                       const IConcreteObjectType* object) {
  
  vector<vector<llvm::Constant*>> vTables;
  
  addTypeListInfo(context, object, vTables);
  addDestructorInfo(context, object, vTables);
  addUnthunkInfo(context, object, vTables);
  addInterfaceMapFunctionsToVTable(declareInterfaceMapFunctions(context, object), vTables);
  defineVTableGlobal(context, object, vTables);
}

void IConcreteObjectType::declareVTable(IRGenerationContext& context,
                                        const IConcreteObjectType* object) {
  
  vector<unsigned long> vTableDimensions;
  
  vector<list<llvm::Constant*>> mapFunction = declareInterfaceMapFunctions(context, object);
  for (list<llvm::Constant*> mapFunctionsPortion : mapFunction) {
    vTableDimensions.push_back(3u + mapFunctionsPortion.size());
  }
  if (!mapFunction.size()) {
    vTableDimensions.push_back(3u);
  }
  declareVTableGlobal(context, object, vTableDimensions);
}

map<string, Function*> IConcreteObjectType::declareMethodFunctions(IRGenerationContext& context,
                                                                   const IConcreteObjectType*
                                                                   object) {
  map<string, Function*> methodFunctionMap;
  vector<tuple<IMethod*, Function*>> methodsWithFunctions;
  
  for (IMethod* method : object->getMethods()) {
    Function* function = method->declareFunction(context);
    methodFunctionMap[method->getName()] = function;
    methodsWithFunctions.push_back(make_tuple(method, function));
  }
  
  return methodFunctionMap;
}

void IConcreteObjectType::addTypeListInfo(IRGenerationContext& context,
                                          const IConcreteObjectType* object,
                                          vector<vector<llvm::Constant*>>& vTables) {
  GlobalVariable* typeListGlobal = createTypeListGlobal(context, object);
  
  llvm::PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<llvm::Constant*> vTablePortion;
  
  vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
  vTablePortion.push_back(ConstantExpr::getBitCast(typeListGlobal, int8Pointer));
  
  vTables.push_back(vTablePortion);
}

void IConcreteObjectType::addDestructorInfo(IRGenerationContext& context,
                                            const IConcreteObjectType* object,
                                            vector<vector<llvm::Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  Function* destructor = declareDestructor(context, object);
  
  vTables.at(0).push_back(ConstantExpr::getBitCast(destructor, int8Pointer));
}

Function* IConcreteObjectType::declareDestructor(IRGenerationContext& context,
                                                 const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  vector<Type*> argumentTypes;
  argumentTypes.push_back(int8Pointer);
  argumentTypes.push_back(thread->getLLVMType(context));
  argumentTypes.push_back(callstack->getLLVMType(context));
  argumentTypes.push_back(int8Pointer);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);
  string functionName = getObjectDestructorFunctionName(object);
  
  return Function::Create(functionType,
                          GlobalValue::ExternalLinkage,
                          functionName,
                          context.getModule());
}

void IConcreteObjectType::scheduleDestructorBodyComposition(IRGenerationContext& context,
                                                            const IConcreteObjectType* object) {
  Function* function = context.getModule()->getFunction(getObjectDestructorFunctionName(object));
  if (object->isPooled()) {
    context.addComposingCallback1Objects(composePooledObjectDestructorBody, function, object);
  } else {
    context.addComposingCallback1Objects(composeDestructorBody, function, object);
  }
}

void IConcreteObjectType::addUnthunkInfo(IRGenerationContext& context,
                                         const IConcreteObjectType* object,
                                         vector<vector<llvm::Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  unsigned long vTableSize = getVTableSizeForObject(object);
  
  for (unsigned long i = 1; i < vTableSize; i++) {
    vector<llvm::Constant*> vTablePortion;
    
    long unthunkBy = -Environment::getAddressSizeInBytes() * i;
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTablePortion.push_back(ConstantExpr::getIntToPtr(unthunk, int8Pointer));
    vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
    vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
    
    vTables.push_back(vTablePortion);
  }
}

vector<list<llvm::Constant*>> IConcreteObjectType::
declareInterfaceMapFunctions(IRGenerationContext& context, const IConcreteObjectType* object) {
  map<string, Function*> methodFunctionMap = declareMethodFunctions(context, object);
  
  vector<list<llvm::Constant*>> interfaceMapFunctions;
  
  for (Interface* interface : object->getInterfaces()) {
    vector<list<llvm::Constant*>> vSubTable =
    interface->declareMapFunctions(context,
                                   object,
                                   methodFunctionMap,
                                   interfaceMapFunctions.size());
    for (list<llvm::Constant*> vTablePortion : vSubTable) {
      interfaceMapFunctions.push_back(vTablePortion);
    }
  }
  
  return interfaceMapFunctions;
}

void IConcreteObjectType::
addInterfaceMapFunctionsToVTable(vector<list<llvm::Constant*>> interfaceMapFunctions,
                                 vector<vector<llvm::Constant*>>& vTables) {
  assert(interfaceMapFunctions.size() == 0 || interfaceMapFunctions.size() == vTables.size());
  
  int vTablesIndex = 0;
  for (list<llvm::Constant*> interfaceMapFunctionsPortion : interfaceMapFunctions) {
    for (llvm::Constant* constant : interfaceMapFunctionsPortion) {
      vTables.at(vTablesIndex).push_back(constant);
    }
    vTablesIndex++;
  }
}

void IConcreteObjectType::composeInterfaceMapFunctions(IRGenerationContext& context,
                                                       const IConcreteObjectType* object) {
  unsigned long offset = 0;
  for (Interface* interface : object->getInterfaces()) {
    offset += interface->composeMapFunctions(context, object, offset);
  }
}

void IConcreteObjectType::defineVTableGlobal(IRGenerationContext& context,
                                             const IConcreteObjectType* object,
                                             vector<vector<llvm::Constant*>> interfaceVTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<llvm::Constant*> vTableArray;
  vector<Type*> vTableTypes;
  for (vector<llvm::Constant*> vTablePortionVector : interfaceVTables) {
    llvm::ArrayType* arrayType = llvm::ArrayType::get(int8Pointer, vTablePortionVector.size());
    llvm::Constant* constantArray = ConstantArray::get(arrayType, vTablePortionVector);
    
    vTableArray.push_back(constantArray);
    vTableTypes.push_back(arrayType);
  }
  
  StructType* vTableGlobalType = StructType::get(llvmContext, vTableTypes);
  llvm::Constant* vTableGlobalConstantStruct = ConstantStruct::get(vTableGlobalType, vTableArray);
  
  new GlobalVariable(*context.getModule(),
                     vTableGlobalType,
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     vTableGlobalConstantStruct,
                     object->getVTableName());
}

void IConcreteObjectType::declareVTableGlobal(IRGenerationContext& context,
                                              const IConcreteObjectType* object,
                                              vector<unsigned long> tableDimensions) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Type*> vTableTypes;
  for (unsigned long dimension : tableDimensions) {
    llvm::ArrayType* arrayType = llvm::ArrayType::get(int8Pointer, dimension);
    vTableTypes.push_back(arrayType);
  }
  
  StructType* vTableGlobalType = StructType::get(llvmContext, vTableTypes);
  
  new GlobalVariable(*context.getModule(),
                     vTableGlobalType,
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     NULL,
                     object->getVTableName());
}

GlobalVariable* IConcreteObjectType::createTypeListGlobal(IRGenerationContext& context,
                                                          const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  llvm::Constant* objectTypeNamePointer = object->getObjectTypeNameGlobal(context);
  llvm::Constant* objectNamePointer = IObjectType::getObjectNamePointer(object, context);
  
  vector<llvm::Constant*> typeNames;
  typeNames.push_back(objectTypeNamePointer);
  typeNames.push_back(objectNamePointer);
  
  for (Interface* interface : interfaces) {
    llvm::Constant* interfaceNamePointer = IObjectType::getObjectNamePointer(interface, context);
    typeNames.push_back(interfaceNamePointer);
  }
  typeNames.push_back(ConstantExpr::getNullValue(int8Pointer));
  llvm::ArrayType* arrayType = llvm::ArrayType::get(int8Pointer, typeNames.size());
  llvm::Constant* constantArray = ConstantArray::get(arrayType, typeNames);
  
  return new GlobalVariable(*context.getModule(),
                            arrayType,
                            true,
                            GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                            constantArray,
                            object->getTypeTableName());
}

void IConcreteObjectType::declareFieldVariables(IRGenerationContext& context,
                                                const IConcreteObjectType* object) {
  for (IField* field : object->getFields()) {
    field->getType()->createFieldVariable(context, field->getName(), object, field->getLine());
  }
}

void IConcreteObjectType::composeDestructorBody(IRGenerationContext& context,
                                                Function* function,
                                                const void* object) {
  const IConcreteObjectType* concreteObject = (const IConcreteObjectType*) object;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  context.getScopes().pushScope();
  context.setBasicBlock(basicBlock);
  
  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);

  Function::arg_iterator functionArguments = function->arg_begin();
  Value* thisGeneric = &*functionArguments;
  thisGeneric->setName(IObjectType::THIS);
  functionArguments++;
  Value* threadArgument = &*functionArguments;
  threadArgument->setName(ThreadExpression::THREAD);
  functionArguments++;
  llvm::Argument* callstackArgument = &*functionArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  functionArguments++;
  Value* exception = &*functionArguments;
  exception->setName("exception");
  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   thread,
                                                                   threadArgument,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstack,
                                                                      callstackArgument,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  Value* nullValue = ConstantPointerNull::get((llvm::PointerType*) thisGeneric->getType());
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisGeneric, nullValue, "");
  
  BasicBlock* ifThisIsNullBlock = BasicBlock::Create(llvmContext, "if.this.null", function);
  BasicBlock* ifThisIsNotNullBlock = BasicBlock::Create(llvmContext, "if.this.notnull", function);
  
  IRWriter::createConditionalBranch(context, ifThisIsNullBlock, ifThisIsNotNullBlock, isNull);
  
  context.setBasicBlock(ifThisIsNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifThisIsNotNullBlock);
  
  Value* thisValue = IRWriter::newBitCastInst(context,
                                              thisGeneric,
                                              concreteObject->getLLVMType(context));
  
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    StringLiteral* stringLiteral =
    new StringLiteral("destructor " + concreteObject->getTypeName() + "\n", 0);
    printOutArguments.push_back(stringLiteral);
    PrintOutStatement::printExpressionList(context, printOutArguments, 0);
  }
  
  decrementReferenceFields(context, thisValue, concreteObject);
  freeOwnerFields(context, thisValue, concreteObject, exception, 0);
  
  Value* referenceCount = concreteObject->getReferenceCount(context, thisValue);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  Value* objectName = IObjectType::getObjectNamePointer(concreteObject, context);
  ThrowReferenceCountExceptionFunction::call(context, referenceCount, objectName, exception);
  IRWriter::newUnreachableInst(context);

  context.setBasicBlock(refCountZeroBlock);
  
  const Controller* cMemoryPool = context.getController(Names::getCMemoryPoolFullName(), 0);
  if (concreteObject == cMemoryPool) {
    addMemoryPoolDestructor(context, function, exception, thisValue);
  }
  
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext),
                              -Environment::getAddressSizeInBytes());
  Value* refCounterObject = IRWriter::createGetElementPtrInst(context, thisGeneric, index);
  IRWriter::createFree(context, refCounterObject);
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    StringLiteral* stringLiteral =
    new StringLiteral("done destructing " + concreteObject->getTypeName() + "\n", 0);
    printOutArguments.push_back(stringLiteral);
    PrintOutStatement::printExpressionList(context, printOutArguments, 0);
  }
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}

void IConcreteObjectType::composePooledObjectDestructorBody(IRGenerationContext& context,
                                                            Function* function,
                                                            const void* object) {
  const IConcreteObjectType* concreteObject = (const IConcreteObjectType*) object;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  context.getScopes().pushScope();
  context.setBasicBlock(basicBlock);
  
  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);

  Function::arg_iterator functionArguments = function->arg_begin();
  Value* thisGeneric = &*functionArguments;
  thisGeneric->setName(IObjectType::THIS);
  functionArguments++;
  Value* threadArgument = &*functionArguments;
  threadArgument->setName(ThreadExpression::THREAD);
  functionArguments++;
  llvm::Argument* callstackArgument = &*functionArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  functionArguments++;
  Value* exception = &*functionArguments;
  exception->setName("exception");
  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   thread,
                                                                   threadArgument,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstack,
                                                                      callstackArgument,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  Value* nullValue = ConstantPointerNull::get((llvm::PointerType*) thisGeneric->getType());
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisGeneric, nullValue, "");
  
  BasicBlock* ifThisIsNullBlock = BasicBlock::Create(llvmContext, "if.this.null", function);
  BasicBlock* ifThisIsNotNullBlock = BasicBlock::Create(llvmContext, "if.this.notnull", function);
  
  IRWriter::createConditionalBranch(context, ifThisIsNullBlock, ifThisIsNotNullBlock, isNull);
  
  context.setBasicBlock(ifThisIsNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifThisIsNotNullBlock);
  
  Value* thisValue = IRWriter::newBitCastInst(context,
                                              thisGeneric,
                                              concreteObject->getLLVMType(context));
  
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    StringLiteral* stringLiteral =
    new StringLiteral("destructor pooled object " + concreteObject->getTypeName() + "\n", 0);
    printOutArguments.push_back(stringLiteral);
    PrintOutStatement::printExpressionList(context, printOutArguments, 0);
  }
  
  decrementReferenceFields(context, thisValue, concreteObject);
  freeOwnerFields(context, thisValue, concreteObject, exception, 0);
  
  Value* referenceCount = concreteObject->getReferenceCount(context, thisValue);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  Value* objectName = IObjectType::getObjectNamePointer(concreteObject, context);
  ThrowReferenceCountExceptionFunction::call(context, referenceCount, objectName, exception);
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(refCountZeroBlock);
  
  unsigned long interfacesCount = concreteObject->getFlattenedInterfaceHierarchy().size();
  unsigned long poolOffset = interfacesCount > 0 ? interfacesCount : 1;
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), poolOffset);
  Value* poolStore = IRWriter::createGetElementPtrInst(context, thisValue, index);
  Value* poolUncast = IRWriter::newLoadInst(context, poolStore, "");
  
  Value* pool = IRWriter::newBitCastInst(context,
                                         poolUncast,
                                         getCMemoryPoolStruct(context)->getPointerTo());
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* countStore = IRWriter::createGetElementPtrInst(context, pool, index);
  Value* count = IRWriter::newLoadInst(context, countStore, "");
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  Value* countMinusOne = IRWriter::createBinaryOperator(context,
                                                        Instruction::Sub,
                                                        count,
                                                        one,
                                                        "");
  IRWriter::newStoreInst(context, countMinusOne, countStore);
  BasicBlock* poolCountZeroBlock = BasicBlock::Create(llvmContext, "pool.count.zero", function);
  BasicBlock* poolCountNotZeroBlock = BasicBlock::Create(llvmContext,
                                                         "pool.count.notzero",
                                                         function);
  isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, countMinusOne, zero, "");
  IRWriter::createConditionalBranch(context, poolCountZeroBlock, poolCountNotZeroBlock, isZero);

  context.setBasicBlock(poolCountZeroBlock);
  const Controller* cMemoryPool = context.getController(Names::getCMemoryPoolFullName(), 0);
  FakeExpression* poolMapExpression = new FakeExpression(poolUncast, cMemoryPool);
  IdentifierChain* clearMethod =
  new IdentifierChain(poolMapExpression, Names::getClearMethodName(), 0);
  ExpressionList clearArguments;
  MethodCall* clearCall = MethodCall::create(clearMethod, clearArguments, 0);
  clearCall->generateIR(context, PrimitiveTypes::VOID);
  IRWriter::createBranch(context, poolCountNotZeroBlock);
  
  context.setBasicBlock(poolCountNotZeroBlock);

  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    StringLiteral* stringLiteral =
    new StringLiteral("done destructing pooled object " + concreteObject->getTypeName() + "\n", 0);
    printOutArguments.push_back(stringLiteral);
    PrintOutStatement::printExpressionList(context, printOutArguments, 0);
  }
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
  
  delete clearCall;
}

void IConcreteObjectType::decrementReferenceFields(IRGenerationContext& context,
                                                   Value* thisValue,
                                                   const IConcreteObjectType* object) {
  for (IField* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isReference()) {
      continue;
    }

    if (context.isDestructorDebugOn()) {
      ExpressionList printOutArguments;
      StringLiteral* stringLiteral =
      new StringLiteral("decrementing " + object->getTypeName() + "." + field->getName() + "\n", 0);
      printOutArguments.push_back(stringLiteral);
      PrintOutStatement::printExpressionList(context, printOutArguments, 0);
    }

    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);
    const IReferenceType* referenceType = (const IReferenceType*) fieldType;
    referenceType->decrementReferenceCount(context, fieldValuePointer);
  }
}

void IConcreteObjectType::freeOwnerFields(IRGenerationContext& context,
                                          Value* thisValue,
                                          const IConcreteObjectType* object,
                                          Value* exception,
                                          int line) {
  for (IField* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isOwner()) {
      continue;
    }
 
    if (context.isDestructorDebugOn()) {
      ExpressionList printOutArguments;
      StringLiteral* stringLiteral =
      new StringLiteral("freeing " + object->getTypeName() + "." + field->getName() + "\n", 0);
      printOutArguments.push_back(stringLiteral);
      PrintOutStatement::printExpressionList(context, printOutArguments, 0);
    }
    
    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);

    if (field->isInjected()) {
      ((InjectedField*) field)->free(context, fieldValuePointer, exception, line);
      continue;
    }

    const IOwnerType* ownerType = (const IOwnerType*) fieldType;
    ownerType->free(context, fieldValuePointer, exception, NULL, line);
  }
}

Value* IConcreteObjectType::getFieldValuePointer(IRGenerationContext& context,
                                                 Value* thisValue,
                                                 const IConcreteObjectType* object,
                                                 IField* field) {
  Value* fieldPointer = getFieldPointer(context, thisValue, object, field);
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* IConcreteObjectType::getFieldPointer(IRGenerationContext& context,
                                            Value* thisValue,
                                            const IConcreteObjectType* object,
                                            IField* field) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
  
  return IRWriter::createGetElementPtrInst(context, thisValue, index);
}

string IConcreteObjectType::getObjectDestructorFunctionName(const IConcreteObjectType* object) {
  return object->getTypeName() + ".destructor";
}

void IConcreteObjectType::composeDestructorCall(IRGenerationContext& context,
                                                const IConcreteObjectType* object,
                                                Value* value,
                                                Value* exception,
                                                int line) {
  string destructorFunctionName = getObjectDestructorFunctionName(object);
  Function* destructor = context.getModule()->getFunction(destructorFunctionName);

  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8pointer);

  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  arguments.push_back(threadVariable->generateIdentifierIR(context, line));
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  arguments.push_back(callstackVariable->generateIdentifierIR(context, line));
  
  if (exception) {
    arguments.push_back(exception);
    IRWriter::createCallInst(context, destructor, arguments, "");
  } else {
    llvm::PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
    Value* nullPointer = ConstantPointerNull::get(int8Pointer);
    arguments.push_back(nullPointer);
    IRWriter::createInvokeInst(context, destructor, arguments, "", line);
  }
}

void IConcreteObjectType::generateStaticMethodsIR(IRGenerationContext& context,
                                                  const IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (method->isStatic() && method->isOverride()) {
      context.reportError(method->getMethodQualifiers()->getLine(),
                          "Static methods can not be marked with override qualifier");
      throw 1;
    }
    if (method->isStatic()) {
      method->generateIR(context);
    }
  }
}

void IConcreteObjectType::generateMethodsIR(IRGenerationContext& context,
                                            const IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (method->isOverride()) {
      checkMethodOverride(context, object, method);
    }
    if (!method->isStatic()) {
      method->generateIR(context);
    }
  }
}

void IConcreteObjectType::checkMethodOverride(IRGenerationContext& context,
                                              const IConcreteObjectType* object,
                                              IMethod* method) {
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  for (Interface* interface : interfaces) {
    if (interface->findMethod(method->getName())) {
      return;
    }
  }
  
  context.reportError(method->getMethodQualifiers()->getLine(),
                      "Method '" + method->getName() + "' of object " + object->getTypeName()
                      + " is marked override but does not override any interface methods");
  throw 1;
}

void IConcreteObjectType::declareConstants(IRGenerationContext& context,
                                           const IConcreteObjectType* object) {
  for (Constant* constant : object->getConstants()) {
    constant->declare(context, object);
  }
}

void IConcreteObjectType::defineConstants(IRGenerationContext& context,
                                          const IConcreteObjectType* object) {
  for (Constant* constant : object->getConstants()) {
    constant->define(context, object);
  }
}

void IConcreteObjectType::declareLLVMFunctions(IRGenerationContext& context,
                                               const IConcreteObjectType* object) {
  for (LLVMFunction* llvmFunction : object->getLLVMFunctions()) {
    llvmFunction->declareFunction(context, object);
  }
}

void IConcreteObjectType::generateLLVMFunctionsIR(IRGenerationContext& context,
                                                  const IConcreteObjectType* object) {
  for (LLVMFunction* llvmFunction : object->getLLVMFunctions()) {
    llvmFunction->generateBodyIR(context, object);
  }
}

void IConcreteObjectType::printObjectToStream(IRGenerationContext& context,
                                              const IConcreteObjectType* object,
                                              iostream& stream) {
  stream << "external ";
  printTypeKind(object, stream);
  stream << " ";
  stream << object->getTypeName();
  if (object->isPooled()) {
    stream << " onPool";
  }
  if (!object->isPublic()) {
    stream << " {" << endl << "}" << endl;
    return;
  }
  
  vector<Interface*> interfaces = object->getInterfaces();
  if (interfaces.size()) {
    stream << endl << "  implements";
  }
  for (Interface* interface : interfaces) {
    stream << endl << "    " << interface->getTypeName();
    if (interface != interfaces.at(interfaces.size() - 1)) {
      stream << ",";
    }
  }
  stream << " {" << endl;
  
  if (object->getConstants().size()) {
    stream << endl;
  }
  for (Constant* constant : object->getConstants()) {
    constant->printToStream(context, stream);
  }

  vector<IField*> fields = object->isController()
  ? ((const Controller*) object)->getReceivedFields()
  : object->getFields();
  if (fields.size()) {
    stream << endl;
  }
  for (IField* field : fields) {
    field->printToStream(context, stream);
  }
  
  if (object->getMethods().size()) {
    stream << endl;
  }
  for (IMethod* method : object->getMethods()) {
    if (method->isPublic()) {
      method->printToStream(context, stream);
    }
  }
  
  stream << "}" << endl;
}

void IConcreteObjectType::printTypeKind(const IConcreteObjectType* type, iostream& stream) {
  if (type->isModel()) {
    stream << "model";
  } else if (type->isNode()) {
    stream << "node";
  } else if (type->isInterface()) {
    stream << "interface";
  } else {
    stream << "controller";
  }
}

void IConcreteObjectType::addInterfaceAndItsParents(Interface* interface,
                                                    vector<Interface *> &result) {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(interface, result);
  }
}

bool IConcreteObjectType::canCast(const IType* fromType, const IType* toType) {
  if (fromType == toType) {
    return true;
  }
  if (toType->isInterface() &&
      getInterfaceIndex((const IConcreteObjectType*) fromType, (const Interface*) toType) >= 0) {
    return true;
  }
  if (toType->isNative() && toType->isPointer()) {
    return true;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }
  return false;
}

unsigned long IConcreteObjectType::getVTableSizeForObject(const IConcreteObjectType* object) {
  unsigned long size = object->getFlattenedInterfaceHierarchy().size();
  return size ? size : 1;
}

llvm::Instruction* IConcreteObjectType::createMallocForObject(IRGenerationContext& context,
                                                              const IConcreteObjectType* object,
                                                              string variableName) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* structType = getOrCreateRefCounterStruct(context, object);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, one, variableName);
  IntrinsicFunctions::setMemoryToZero(context, malloc, ConstantExpr::getSizeOf(structType));
  return malloc;
}

llvm::StructType* IConcreteObjectType::
getOrCreateRefCounterStruct(IRGenerationContext& context, const IConcreteObjectType* object) {
  string structName = object->getTypeName() + ".refCounter";
  StructType* structType = context.getModule()->getTypeByName(structName);
  if (structType) {
    return structType;
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  structType = StructType::create(llvmContext, structName);
  vector<Type*> types;
  types.push_back(Type::getInt64Ty(llvmContext));
  types.push_back(object->getLLVMType(context)->getPointerElementType());
  structType->setBody(types);

  return structType;
}

const IMethod* IConcreteObjectType::findMethodInObject(string methodName,
                                                       const IConcreteObjectType* object) {
  std::map<std::string, IMethod*> nameToMethodMap = object->getNameToMethodMap();
  if (nameToMethodMap.count(methodName)) {
    return nameToMethodMap.at(methodName);
  }
  
  for (Interface* interface : object->getFlattenedInterfaceHierarchy()) {
    IMethod* method = interface->findStaticMethod(methodName);
    if (method) {
      return method;
    }
  }
  
  return NULL;
}

StructType* IConcreteObjectType::getCMemoryPoolStruct(IRGenerationContext& context) {
  string structName = "CMemoryPool";
  StructType* structType = context.getModule()->getTypeByName(structName);
  if (structType) {
    return structType;
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  StructType* aprPool = context.getModule()->getTypeByName("AprPool");
  assert(aprPool && "Could not find AprPool struct");
  structType = StructType::create(llvmContext, structName);
  vector<Type*> types;
  types.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  types.push_back(Type::getInt64Ty(llvmContext));
  types.push_back(aprPool->getPointerTo());
  structType->setBody(types);
  
  return structType;
}

void IConcreteObjectType::addMemoryPoolDestructor(IRGenerationContext& context,
                                                  Function* function,
                                                  Value* exception,
                                                  Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* objectCountZeroBlock = BasicBlock::Create(llvmContext, "object.count.zero", function);
  BasicBlock* objectCountNotZeroBlock = BasicBlock::Create(llvmContext,
                                                           "object.count.not.zero",
                                                           function);
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Type* memoryPoolType = getCMemoryPoolStruct(context)->getPointerTo();
  Value* memoryPool = IRWriter::newBitCastInst(context, object, memoryPoolType);
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* objectCountStore = IRWriter::createGetElementPtrInst(context, memoryPool, index);
  Value* objectCount = IRWriter::newLoadInst(context, objectCountStore, "objectCount");
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, objectCount, zero, "");
  IRWriter::createConditionalBranch(context, objectCountZeroBlock, objectCountNotZeroBlock, isZero);

  context.setBasicBlock(objectCountNotZeroBlock);
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getMemoryPoolNonEmptyExceptionName(), 0);
  BuilderArgumentList builderArgumnetList;
  FakeExpression* fakeExpression = new FakeExpression(objectCount, PrimitiveTypes::LONG);
  BuilderArgument* refCountArgument = new BuilderArgument("withObjectCount", fakeExpression);
  Interface* exceptionInterface = context.getInterface(Names::getExceptionInterfaceFullName(), 0);
  Value* exceptionCast = IRWriter::newBitCastInst(context,
                                                  exception,
                                                  exceptionInterface->getLLVMType(context));
  fakeExpression = new FakeExpression(exceptionCast, exceptionInterface);
  BuilderArgument* nestedExceptionArgument =
  new BuilderArgument("withNestedException", fakeExpression);
  builderArgumnetList.push_back(refCountArgument);
  builderArgumnetList.push_back(nestedExceptionArgument);
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier, builderArgumnetList, 0);
  ThrowStatement throwStatement(heapBuilder, 0);
  throwStatement.generateIR(context);
  IRWriter::newUnreachableInst(context);

  context.setBasicBlock(objectCountZeroBlock);
  const Controller* cMemoryPool = context.getController(Names::getCMemoryPoolFullName(), 0);
  FakeExpression* poolMapExpression = new FakeExpression(object, cMemoryPool);
  IdentifierChain* destroyMethod =
  new IdentifierChain(poolMapExpression, Names::getDestroyMethodName(), 0);
  ExpressionList destroyArguments;
  MethodCall* destroyCall = MethodCall::create(destroyMethod, destroyArguments, 0);
  destroyCall->generateIR(context, PrimitiveTypes::VOID);
  
  delete destroyCall;
}
