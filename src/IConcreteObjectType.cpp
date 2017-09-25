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

#include "wisey/Cast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IField.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/OwnerFieldVariable.hpp"
#include "wisey/PrimitiveFieldVariable.hpp"
#include "wisey/ReferenceFieldVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IConcreteObjectType::generateNameGlobal(IRGenerationContext& context,
                                             IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, object->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     object->getObjectNameGlobalVariableName());
}

Value* IConcreteObjectType::castTo(IRGenerationContext& context,
                                   IConcreteObjectType* object,
                                   Value* fromValue,
                                   const IType* toType) {
  if (toType == object) {
    return fromValue;
  }
  if (!object->canCastTo(toType)) {
    Cast::exitIncompatibleTypes(object, toType);
    return NULL;
  }
  LLVMContext& llvmContext = context.getLLVMContext();
  Interface* interface = (Interface*) toType;
  Type* llvmType = (PointerType*) interface->getLLVMType(llvmContext);
  int interfaceIndex = getInterfaceIndex(object, interface);
  if (interfaceIndex == 0) {
    return IRWriter::newBitCastInst(context, fromValue, llvmType);
  }
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Value* loadedValue = IRWriter::newLoadInst(context, fromValue, "");
  BitCastInst* bitcast = IRWriter::newBitCastInst(context, loadedValue, int8Type->getPointerTo());
  Value* index[1];
  unsigned long thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  Value* store = IRWriter::newAllocaInst(context, llvmType->getPointerElementType(), "");
  Value* result = IRWriter::newBitCastInst(context, thunk, llvmType->getPointerElementType());
  IRWriter::newStoreInst(context, result, store);
  
  return store;
}

int IConcreteObjectType::getInterfaceIndex(const IConcreteObjectType* object,
                                           Interface* interface) {
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
                                           IConcreteObjectType* object,
                                           Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* vTableGlobal = context.getModule()->getGlobalVariable(object->getVTableName());
  
  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  for (unsigned int vTableIndex = 0; vTableIndex < object->getVTableSize(); vTableIndex++) {
    Value* vTableStart;
    if (vTableIndex == 0) {
      vTableStart = malloc;
    } else {
      Value* vTableStartCalculation = IRWriter::newBitCastInst(context, malloc, genericPointerType);
      Value* index[1];
      unsigned int thunkBy = vTableIndex * Environment::getAddressSizeInBytes();
      index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
      vTableStart = IRWriter::createGetElementPtrInst(context, vTableStartCalculation, index);
    }
    
    Value* vTablePointer =
    IRWriter::newBitCastInst(context, vTableStart, vTableType->getPointerTo());
    Value* index[3];
    index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), vTableIndex);
    index[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = IRWriter::createGetElementPtrInst(context, vTableGlobal, index);
    BitCastInst* bitcast = IRWriter::newBitCastInst(context, initializerStart, vTableType);
    IRWriter::newStoreInst(context, bitcast, vTablePointer);
  }
}

void IConcreteObjectType::generateVTable(IRGenerationContext& context,
                                         IConcreteObjectType* object) {

  vector<vector<Constant*>> vTables;

  addTypeListInfo(context, object, vTables);
  addDestructorInfo(context, object, vTables);
  addUnthunkInfo(context, object, vTables);
  generateInterfaceMapFunctions(context, object, vTables);
  createVTableGlobal(context, object, vTables);
}

map<string, Function*> IConcreteObjectType::generateMethodFunctions(IRGenerationContext& context,
                                                                    IConcreteObjectType* object) {
  map<string, Function*> methodFunctionMap;
  vector<tuple<IMethod*, Function*>> methodsWithFunctions;
  
  for (IMethod* method : object->getMethods()) {
    Function* function = method->defineFunction(context, object);
    methodFunctionMap[method->getName()] = function;
    methodsWithFunctions.push_back(make_tuple(method, function));
  }
  
  return methodFunctionMap;
}

void IConcreteObjectType::addTypeListInfo(IRGenerationContext& context,
                                          IConcreteObjectType* object,
                                          vector<vector<Constant*>>& vTables) {
  GlobalVariable* typeListGlobal = createTypeListGlobal(context, object);
  
  PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTablePortion;

  vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
  vTablePortion.push_back(ConstantExpr::getBitCast(typeListGlobal, int8Pointer));
  
  vTables.push_back(vTablePortion);
}

void IConcreteObjectType::addDestructorInfo(IRGenerationContext& context,
                                            IConcreteObjectType* object,
                                            vector<vector<Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(object->getLLVMType(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  string functionName = getObjectDestructorFunctionName(object);
  
  Function* destructor = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          functionName,
                                          context.getModule());
  
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vTables.at(0).push_back(ConstantExpr::getBitCast(destructor, int8Pointer));
}

void IConcreteObjectType::addUnthunkInfo(IRGenerationContext& context,
                                         IConcreteObjectType* object,
                                         vector<vector<Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  unsigned long vTableSize = object->getVTableSize();
  
  for (unsigned long i = 1; i < vTableSize; i++) {
    vector<Constant*> vTablePortion;
    
    long unthunkBy = -Environment::getAddressSizeInBytes() * i;
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTablePortion.push_back(ConstantExpr::getIntToPtr(unthunk, int8Pointer));
    vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
    vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
    
    vTables.push_back(vTablePortion);
  }
}

void IConcreteObjectType::generateInterfaceMapFunctions(IRGenerationContext& context,
                                                        IConcreteObjectType* object,
                                                        vector<vector<Constant*>>& vTables) {
  map<string, Function*> methodFunctionMap = generateMethodFunctions(context, object);
  
  vector<list<Constant*>> interfaceMapFunctions;
  
  for (Interface* interface : object->getInterfaces()) {
    vector<list<Constant*>> vSubTable =
    interface->generateMapFunctionsIR(context,
                                      object,
                                      methodFunctionMap,
                                      interfaceMapFunctions.size());
    for (list<Constant*> vTablePortion : vSubTable) {
      interfaceMapFunctions.push_back(vTablePortion);
    }
  }
  
  assert(interfaceMapFunctions.size() == 0 || interfaceMapFunctions.size() == vTables.size());
  
  int vTablesIndex = 0;
  for (list<Constant*> interfaceMapFunctionsPortion : interfaceMapFunctions) {
    for (Constant* constant : interfaceMapFunctionsPortion) {
      vTables.at(vTablesIndex).push_back(constant);
    }
    vTablesIndex++;
  }
}

void IConcreteObjectType::createVTableGlobal(IRGenerationContext& context,
                                             IConcreteObjectType* object,
                                             vector<vector<Constant*>> interfaceVTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Constant*> vTableArray;
  vector<Type*> vTableTypes;
  for (vector<Constant*> vTablePortionVector : interfaceVTables) {
    ArrayRef<Constant*> arrayRef(vTablePortionVector);
    ArrayType* arrayType = ArrayType::get(int8Pointer, vTablePortionVector.size());
    Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
    
    vTableArray.push_back(constantArray);
    vTableTypes.push_back(arrayType);
  }
  
  StructType* vTableGlobalType = StructType::get(llvmContext, vTableTypes);
  Constant* vTableGlobalConstantStruct = ConstantStruct::get(vTableGlobalType, vTableArray);
  
  new GlobalVariable(*context.getModule(),
                     vTableGlobalType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     vTableGlobalConstantStruct,
                     object->getVTableName());
}

GlobalVariable* IConcreteObjectType::createTypeListGlobal(IRGenerationContext& context,
                                                          IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = object->getFlattenedInterfaceHierarchy();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  Constant* controllerNamePointer = IObjectType::getObjectNamePointer(object, context);
  
  vector<Constant*> typeNames;
  typeNames.push_back(controllerNamePointer);
  
  for (Interface* interface : interfaces) {
    Constant* interfaceNamePointer = IObjectType::getObjectNamePointer(interface, context);
    typeNames.push_back(interfaceNamePointer);
  }
  typeNames.push_back(ConstantExpr::getNullValue(int8Pointer));
  ArrayRef<Constant*> arrayRef(typeNames);
  ArrayType* arrayType = ArrayType::get(int8Pointer, typeNames.size());
  Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
  
  return new GlobalVariable(*context.getModule(),
                            arrayType,
                            true,
                            GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                            constantArray,
                            object->getTypeTableName());
}

void IConcreteObjectType::declareFieldVariables(IRGenerationContext& context, IConcreteObjectType* object) {
  map<string, IField*> fields = object->getFields();
  for (map<string, IField*>::const_iterator iterator = fields.begin();
       iterator != fields.end();
       iterator++) {
    IField* field = iterator->second;
    const IType* type = field->getType();
    IFieldVariable* fieldVariable = NULL;
    if (IType::isOwnerType(type)) {
      fieldVariable = new OwnerFieldVariable(field->getName(), NULL, object);
    } else if (IType::isReferenceType(type)) {
      fieldVariable = new ReferenceFieldVariable(field->getName(), NULL, object);
    } else {
      fieldVariable = new PrimitiveFieldVariable(field->getName(), NULL, object);
    }

    context.getScopes().setVariable(fieldVariable);
  }
}

void IConcreteObjectType::composeDestructorBody(IRGenerationContext& context,
                                                IConcreteObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getModule()->getFunction(getObjectDestructorFunctionName(object));

  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  Value* thisLoaded = IRWriter::newLoadInst(context, thisArgument, "");
  
  Value* nullValue = ConstantPointerNull::get((PointerType*) thisLoaded->getType());
  Value* condition = IRWriter::newICmpInst(context,
                                           ICmpInst::ICMP_EQ,
                                           thisLoaded,
                                           nullValue,
                                           "");

  BasicBlock* ifThisIsNullBlock = BasicBlock::Create(llvmContext, "if.this.null", function);
  BasicBlock* ifThisIsNotNullBlock = BasicBlock::Create(llvmContext, "if.this.notnull", function);

  IRWriter::createConditionalBranch(context, ifThisIsNullBlock, ifThisIsNotNullBlock, condition);

  context.setBasicBlock(ifThisIsNullBlock);
  IRWriter::createReturnInst(context, NULL);

  context.setBasicBlock(ifThisIsNotNullBlock);
  
  map<string, IField*> fields = object->getFields();
  for (map<string, IField*>::const_iterator iterator = fields.begin();
       iterator != fields.end();
       iterator++) {
    IField* field = iterator->second;
    const IType* fieldType = field->getType();
    if (!IType::isOwnerType(fieldType)) {
      continue;
    }
    const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) fieldType;
    
    Value* index[2];
    index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    
    Value* fieldPointer = IRWriter::createGetElementPtrInst(context, thisLoaded, index);
    Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
    
    BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
    BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
    Value* nullValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext));
    Value* condition = IRWriter::newICmpInst(context,
                                             ICmpInst::ICMP_EQ,
                                             fieldPointerLoaded,
                                             nullValue,
                                             "");
    IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);

    context.setBasicBlock(ifNotNullBlock);
    objectOwnerType->free(context, fieldPointer);
    IRWriter::createBranch(context, ifNullBlock);
    
    context.setBasicBlock(ifNullBlock);
  }
  
  Composer::freeIfNotNull(context, thisLoaded);
  IRWriter::createReturnInst(context, NULL);
}

string IConcreteObjectType::getObjectDestructorFunctionName(IConcreteObjectType* object) {
  return "destructor." + object->getName();
}

void IConcreteObjectType::composeDestructorCall(IRGenerationContext& context,
                                                IConcreteObjectType* object,
                                                Value* value) {
  string destructorFunctionName = getObjectDestructorFunctionName(object);
  Function* function = context.getModule()->getFunction(destructorFunctionName);
  vector<Value*> arguments;
  arguments.push_back(value);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

void IConcreteObjectType::generateStaticMethodsIR(IRGenerationContext& context,
                                                  IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (method->isStatic()) {
      method->generateIR(context, object);
    }
  }
}

void IConcreteObjectType::generateMethodsIR(IRGenerationContext& context,
                                            IConcreteObjectType* object) {
  for (IMethod* method : object->getMethods()) {
    if (!method->isStatic()) {
      method->generateIR(context, object);
    }
  }
}

void IConcreteObjectType::printObjectToStream(const IConcreteObjectType* object, iostream& stream) {
  if (object->getTypeKind() == MODEL_TYPE) {
    stream << "model ";
  } else if (object->getTypeKind() == NODE_TYPE) {
    stream << "node ";
  } else {
    stream << "controller ";
  }
  stream << object->getName();
  vector<Interface*> interfaces = object->getInterfaces();
  if (interfaces.size()) {
    stream << endl << "  implements";
  }
  for (Interface* interface : interfaces) {
    stream << endl << "    " << interface->getName();
    if (interface != interfaces.at(interfaces.size() - 1)) {
      stream << ",";
    }
  }
  stream << " {" << endl;

  map<string, IField*> fields = object->getFields();
  if (fields.size()) {
    stream << endl;
  }
  for (map<string, IField*>::const_iterator iterator = fields.begin();
       iterator != fields.end();
       iterator++) {
    iterator->second->printToStream(stream);
  }
  
  if (object->getMethods().size()) {
    stream << endl;
  }
  
  for (IMethod* method : object->getMethods()) {
    method->printToStream(stream);
  }
  stream << "}" << endl;
}

