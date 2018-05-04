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
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/Names.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IConcreteObjectType::generateNameGlobal(IRGenerationContext& context,
                                             const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  string typeName = object->getTypeName();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, typeName);
  new GlobalVariable(*context.getModule(),
                     llvm::ArrayType::get(Type::getInt8Ty(llvmContext), typeName.length() + 1),
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     object->isExternal() ? nullptr : stringConstant,
                     object->getObjectNameGlobalVariableName());
}

void IConcreteObjectType::generateShortNameGlobal(IRGenerationContext& context,
                                                  const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext,
                                                                object->getShortName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     object->getObjectShortNameGlobalVariableName());
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

  for (unsigned int vTableIndex = 0; vTableIndex < object->getVTableSize(); vTableIndex++) {
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

void IConcreteObjectType::generateVTable(IRGenerationContext& context,
                                         const IConcreteObjectType* object) {
  
  vector<vector<llvm::Constant*>> vTables;
  
  addTypeListInfo(context, object, vTables);
  addDestructorInfo(context, object, vTables);
  addUnthunkInfo(context, object, vTables);
  defineInterfaceMapFunctions(context, object, vTables);
  createVTableGlobal(context, object, vTables);
}

map<string, Function*> IConcreteObjectType::defineMethodFunctions(IRGenerationContext& context,
                                                                  const IConcreteObjectType*
                                                                  object) {
  map<string, Function*> methodFunctionMap;
  vector<tuple<IMethod*, Function*>> methodsWithFunctions;
  
  for (IMethod* method : object->getMethods()) {
    Function* function = method->defineFunction(context);
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
  FunctionType* functionType = getDestructorFunctionType(context);
  string functionName = getObjectDestructorFunctionName(object);
  
  Function* destructor = Function::Create(functionType,
                                          GlobalValue::ExternalLinkage,
                                          functionName,
                                          context.getModule());
  
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vTables.at(0).push_back(ConstantExpr::getBitCast(destructor, int8Pointer));
}

FunctionType* IConcreteObjectType::getDestructorFunctionType(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(int8Pointer);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  
  return FunctionType::get(llvmReturnType, argumentTypes, false);
}

void IConcreteObjectType::scheduleDestructorBodyComposition(IRGenerationContext& context,
                                                            const IConcreteObjectType* object) {
  Function* function = context.getModule()->getFunction(getObjectDestructorFunctionName(object));
  context.addComposingCallback1Objects(composeDestructorBody, function, object);
}

void IConcreteObjectType::addUnthunkInfo(IRGenerationContext& context,
                                         const IConcreteObjectType* object,
                                         vector<vector<llvm::Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  unsigned long vTableSize = object->getVTableSize();
  
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

void IConcreteObjectType::defineInterfaceMapFunctions(IRGenerationContext& context,
                                                      const IConcreteObjectType* object,
                                                      vector<vector<llvm::Constant*>>& vTables) {
  map<string, Function*> methodFunctionMap = defineMethodFunctions(context, object);
  
  vector<list<llvm::Constant*>> interfaceMapFunctions;
  
  for (Interface* interface : object->getInterfaces()) {
    vector<list<llvm::Constant*>> vSubTable =
    interface->defineMapFunctions(context, object, methodFunctionMap, interfaceMapFunctions.size());
    for (list<llvm::Constant*> vTablePortion : vSubTable) {
      interfaceMapFunctions.push_back(vTablePortion);
    }
  }
  
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

void IConcreteObjectType::createVTableGlobal(IRGenerationContext& context,
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
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     vTableGlobalConstantStruct,
                     object->getVTableName());
}

GlobalVariable* IConcreteObjectType::createTypeListGlobal(IRGenerationContext& context,
                                                          const IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  llvm::Constant* objectShortNamePointer = getObjectShortNamePointer(object, context);
  llvm::Constant* objectNamePointer = IObjectType::getObjectNamePointer(object, context);
  
  vector<llvm::Constant*> typeNames;
  typeNames.push_back(objectShortNamePointer);
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
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Value* thisGeneric = &*functionArguments;
  thisGeneric->setName(IObjectType::THIS);
  
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
  freeOwnerFields(context, thisValue, concreteObject, 0);
  
  Value* referenceCount = concreteObject->getReferenceCount(context, thisValue);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    printOutArguments.push_back(new StringLiteral("Throwing RCE " + concreteObject->getTypeName(),
                                                  0));
    printOutArguments.push_back(new StringLiteral(" count = ", 0));
    printOutArguments.push_back(new FakeExpression(referenceCount, PrimitiveTypes::LONG));
    printOutArguments.push_back(new StringLiteral("\n", 0));
    PrintOutStatement::printExpressionList(context, printOutArguments, 0);
  }

  ThrowReferenceCountExceptionFunction::call(context, referenceCount);
  IRWriter::newUnreachableInst(context);

  context.setBasicBlock(refCountZeroBlock);
  
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext),
                              -Environment::getAddressSizeInBytes());
  Value* refCounterObject = IRWriter::createGetElementPtrInst(context, thisGeneric, index);
  IRWriter::createFree(context, refCounterObject);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}

void IConcreteObjectType::decrementReferenceFields(IRGenerationContext& context,
                                                   Value* thisValue,
                                                   const IConcreteObjectType* object) {
  for (IField* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isReference() || fieldType->isNative()) {
      continue;
    }
    
    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);
    const IReferenceType* referenceType = (const IReferenceType*) fieldType;
    referenceType->decrementReferenceCount(context, fieldValuePointer);
  }
}

void IConcreteObjectType::freeOwnerFields(IRGenerationContext& context,
                                          Value* thisValue,
                                          const IConcreteObjectType* object,
                                          int line) {
  for (IField* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isOwner()) {
      continue;
    }
    
    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);
    const IOwnerType* ownerType = (const IOwnerType*) fieldType;
    ownerType->free(context, fieldValuePointer, line);
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

void IConcreteObjectType::composeDestructorCall(IRGenerationContext& context, Value* value) {
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8pointer);
  
  DestroyObjectOwnerFunction::call(context, bitcast);
}

Function* IConcreteObjectType::getDestructorFunctionForObject(IRGenerationContext &context,
                                                              const IConcreteObjectType *object,
                                                              int line) {
  string rceFullName = Names::getLangPackageName() + "." + Names::getReferenceCountExceptionName();
  context.getScopes().getScope()->addException(context.getModel(rceFullName, line), line);
  string destructorFunctionName = getObjectDestructorFunctionName(object);
  
  return context.getModule()->getFunction(destructorFunctionName);
}

void IConcreteObjectType::generateStaticMethodsIR(IRGenerationContext& context,
                                                  const IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (method->isStatic() && method->isOverride()) {
      context.reportError(method->getMethodQualifiers()->getLine(),
                          "Static methods can not be marked with override qualifier");
      exit(1);
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
  exit(1);
}

void IConcreteObjectType::generateConstantsIR(IRGenerationContext& context,
                                              const IConcreteObjectType* object) {
  for (Constant* constant : object->getConstants()) {
    constant->generateIR(context, object);
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

  map<string, const IObjectType*> innerObjects = object->getInnerObjects();
  for (map<string, const IObjectType*>::iterator iterator = innerObjects.begin();
       iterator != innerObjects.end();
       iterator++) {
    stream << endl;
    iterator->second->printToStream(context, stream);
  }
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

void IConcreteObjectType::defineCurrentObjectNameVariable(IRGenerationContext& context,
                                                          const IConcreteObjectType* objectType) {
  Value* objectName = IObjectType::getObjectNamePointer(objectType, context);
  ParameterPrimitiveVariable* objectNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentObjectVariableName(),
                                 PrimitiveTypes::STRING,
                                 objectName,
                                 0);
  context.getScopes().setVariable(context, objectNameVariable);
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

llvm::Constant* IConcreteObjectType::getObjectShortNamePointer(const IConcreteObjectType* object,
                                                               IRGenerationContext& context) {
  GlobalVariable* nameGlobal =
  context.getModule()->getNamedGlobal(object->getObjectShortNameGlobalVariableName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
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
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  return IRWriter::createGetElementPtrInst(context, malloc, index);
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

IMethod* IConcreteObjectType::findMethodInObject(string methodName,
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
