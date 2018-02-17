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
#include "wisey/DecrementReferencesInArrayFunction.hpp"
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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
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
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, object->getTypeName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
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
                                   const IType* toType) {
  if (toType == object) {
    return fromValue;
  }
  if (!object->canCastTo(context, toType)) {
    Cast::exitIncompatibleTypes(object, toType);
    return NULL;
  }
  LLVMContext& llvmContext = context.getLLVMContext();
  Interface* interface = (Interface*) toType;
  Type* llvmType = (PointerType*) interface->getLLVMType(context);
  int interfaceIndex = getInterfaceIndex(object, interface);
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = IRWriter::newBitCastInst(context, fromValue, int8Type->getPointerTo());
  Value* index[1];
  unsigned long thunkBy = (interfaceIndex + 1) * Environment::getAddressSizeInBytes();
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
  
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  for (unsigned int vTableIndex = 0; vTableIndex < object->getVTableSize(); vTableIndex++) {
    Value* vTableStart;
    Value* vTableStartCalculation = IRWriter::newBitCastInst(context, malloc, genericPointerType);
    Value* index[1];
    unsigned int thunkBy = (vTableIndex + 1) * Environment::getAddressSizeInBytes();
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
  
  PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
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
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  
  return FunctionType::get(llvmReturnType, argTypesArray, false);
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
    ArrayRef<llvm::Constant*> arrayRef(vTablePortionVector);
    llvm::ArrayType* arrayType = llvm::ArrayType::get(int8Pointer, vTablePortionVector.size());
    llvm::Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
    
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
  ArrayRef<llvm::Constant*> arrayRef(typeNames);
  llvm::ArrayType* arrayType = llvm::ArrayType::get(int8Pointer, typeNames.size());
  llvm::Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
  
  return new GlobalVariable(*context.getModule(),
                            arrayType,
                            true,
                            GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                            constantArray,
                            object->getTypeTableName());
}

void IConcreteObjectType::declareFieldVariables(IRGenerationContext& context,
                                                const IConcreteObjectType* object) {
  for (Field* field : object->getFields()) {
    const IType* type = field->getType();
    IVariable* fieldVariable = NULL;
    if (IType::isOwnerType(type)) {
      fieldVariable = new FieldOwnerVariable(field->getName(), object);
    } else if (IType::isReferenceType(type)) {
      fieldVariable = new FieldReferenceVariable(field->getName(), object);
    } else if (type->getTypeKind() == ARRAY_OWNER_TYPE) {
      fieldVariable = new FieldArrayOwnerVariable(field->getName(), object);
    } else if (type->getTypeKind() == ARRAY_TYPE) {
      fieldVariable = new FieldArrayReferenceVariable(field->getName(), object);
    } else {
      assert(IType::isPrimitveType(type));
      fieldVariable = new FieldPrimitiveVariable(field->getName(), object);
    }
    
    context.getScopes().setVariable(fieldVariable);
  }
}

void IConcreteObjectType::composeDestructorBody(IRGenerationContext& context,
                                                Function* function,
                                                const IObjectType* objectType) {
  const IConcreteObjectType* object = (const IConcreteObjectType*) objectType;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  context.getScopes().pushScope();
  context.setBasicBlock(basicBlock);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisGeneric = &*functionArguments;
  thisGeneric->setName("this");
  
  Value* nullValue = ConstantPointerNull::get((PointerType*) thisGeneric->getType());
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisGeneric, nullValue, "");
  
  BasicBlock* ifThisIsNullBlock = BasicBlock::Create(llvmContext, "if.this.null", function);
  BasicBlock* ifThisIsNotNullBlock = BasicBlock::Create(llvmContext, "if.this.notnull", function);
  
  IRWriter::createConditionalBranch(context, ifThisIsNullBlock, ifThisIsNotNullBlock, isNull);
  
  context.setBasicBlock(ifThisIsNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifThisIsNotNullBlock);
  
  Value* thisValue = IRWriter::newBitCastInst(context,
                                              thisGeneric,
                                              objectType->getLLVMType(context));
  
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    printOutArguments.push_back(new StringLiteral("destructor " + object->getTypeName() + "\n"));
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }
  
  decrementReferenceFields(context, thisValue, object);
  freeOwnerFields(context, thisValue, object);
  
  Value* referenceCount = object->getReferenceCount(context, thisValue);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  ThrowReferenceCountExceptionFunction::call(context, referenceCount);
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(refCountZeroBlock);
  
  IRWriter::createFree(context, thisGeneric);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}

void IConcreteObjectType::decrementReferenceFields(IRGenerationContext& context,
                                                   Value* thisValue,
                                                   const IConcreteObjectType* object) {
  for (Field* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isReference()) {
      continue;
    }
    
    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);
    const IReferenceType* referenceType = (const IReferenceType*) fieldType;
    referenceType->decrementReferenceCount(context, fieldValuePointer);
  }
}

void IConcreteObjectType::freeOwnerFields(IRGenerationContext& context,
                                          Value* thisValue,
                                          const IConcreteObjectType* object) {
  for (Field* field : object->getFields()) {
    const IType* fieldType = field->getType();
    
    if (!fieldType->isOwner()) {
      continue;
    }
    
    Value* fieldValuePointer = getFieldValuePointer(context, thisValue, object, field);
    const IOwnerType* ownerType = (const IOwnerType*) fieldType;
    ownerType->free(context, fieldValuePointer);
  }
}

Value* IConcreteObjectType::getFieldValuePointer(IRGenerationContext& context,
                                                 Value* thisValue,
                                                 const IConcreteObjectType* object,
                                                 Field* field) {
  Value* fieldPointer = getFieldPointer(context, thisValue, object, field);
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* IConcreteObjectType::getFieldPointer(IRGenerationContext& context,
                                            Value* thisValue,
                                            const IConcreteObjectType* object,
                                            Field* field) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
  
  return IRWriter::createGetElementPtrInst(context, thisValue, index);
}

string IConcreteObjectType::getObjectDestructorFunctionName(const IConcreteObjectType* object) {
  return "destructor." + object->getTypeName();
}

void IConcreteObjectType::composeDestructorCall(IRGenerationContext& context,
                                                const IConcreteObjectType* object,
                                                Value* value) {
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8pointer);

  Function* function = getDestructorFunctionForObject(context, object);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

Function* IConcreteObjectType::getDestructorFunctionForObject(IRGenerationContext &context,
                                                              const IConcreteObjectType *object) {
  string rceFullName = Names::getLangPackageName() + "." + Names::getReferenceCountExceptionName();
  context.getScopes().getScope()->addException(context.getModel(rceFullName));
  string destructorFunctionName = getObjectDestructorFunctionName(object);
  
  return context.getModule()->getFunction(destructorFunctionName);
}

void IConcreteObjectType::generateStaticMethodsIR(IRGenerationContext& context,
                                                  const IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (method->isStatic()) {
      method->generateIR(context);
    }
  }
}

void IConcreteObjectType::generateMethodsIR(IRGenerationContext& context,
                                            const IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (!method->isStatic()) {
      method->generateIR(context);
    }
  }
}

void IConcreteObjectType::generateConstantsIR(IRGenerationContext& context,
                                              const IConcreteObjectType* object) {
  for (Constant* constant : object->getConstants()) {
    constant->generateIR(context, object);
  }
}

void IConcreteObjectType::printObjectToStream(IRGenerationContext& context,
                                              const IConcreteObjectType* object,
                                              iostream& stream) {
  stream << "external ";
  printTypeKind(object->getTypeKind(), stream);
  stream << (object->isInner() ? object->getShortName() : object->getTypeName());
  if (object->getAccessLevel() == PRIVATE_ACCESS) {
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

  vector<Field*> fields = object->getFields();
  if (fields.size()) {
    stream << endl;
  }
  for (Field* field : fields) {
    field->printToStream(context, stream);
  }
  
  if (object->getMethods().size()) {
    stream << endl;
  }
  for (IMethod* method : object->getMethods()) {
    if (method->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
      method->printToStream(context, stream);
    }
  }
  
  map<string, const IObjectType*> innerObjects = object->getInnerObjects();
  if (innerObjects.size()) {
    stream << endl;
  }
  for (map<string, const IObjectType*>::iterator iterator = innerObjects.begin();
       iterator != innerObjects.end();
       iterator++) {
    iterator->second->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "}" << endl;
}

void IConcreteObjectType::printTypeKind(TypeKind typeKind, iostream& stream) {
  if (typeKind == MODEL_TYPE) {
    stream << "model ";
  } else if (typeKind == NODE_TYPE) {
    stream << "node ";
  } else if (typeKind == INTERFACE_TYPE) {
    stream << "interface ";
  } else {
    stream << "controller ";
  }
}

void IConcreteObjectType::defineCurrentObjectNameVariable(IRGenerationContext& context,
                                                          const IConcreteObjectType* objectType) {
  Value* objectName = IObjectType::getObjectNamePointer(objectType, context);
  ParameterPrimitiveVariable* objectNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentObjectVariableName(),
                                 PrimitiveTypes::STRING_TYPE,
                                 objectName);
  context.getScopes().setVariable(objectNameVariable);
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
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      getInterfaceIndex((IConcreteObjectType*) fromType, (Interface*) toType) >= 0) {
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

