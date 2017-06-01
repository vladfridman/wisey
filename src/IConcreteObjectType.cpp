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
#include "wisey/Environment.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ObjectFieldVariable.hpp"

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
                                   IType* toType) {
  if (toType == object) {
    return fromValue;
  }
  if (!object->canCastTo(toType)) {
    Cast::exitIncopatibleTypes(object, toType);
    return NULL;
  }
  LLVMContext& llvmContext = context.getLLVMContext();
  Interface* interface = (Interface*) toType;
  unsigned long interfaceIndex = getInterfaceIndex(object, interface);
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = IRWriter::newBitCastInst(context, fromValue, int8Type->getPointerTo());
  Value* index[1];
  unsigned long thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  return IRWriter::newBitCastInst(context, thunk, interface->getLLVMType(llvmContext));
}

unsigned long IConcreteObjectType::getInterfaceIndex(IConcreteObjectType* object,
                                                     Interface* interface) {
  int index = 1;
  for (Interface* implementedInterface : object->getFlattenedInterfaceHierarchy()) {
    if (implementedInterface == interface) {
      return index;
    }
    index++;
  }
  return 0;
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
  addUnthunkInfo(context, object, vTables);
  generateInterfaceMapFunctions(context, object, vTables);
  createVTableGlobal(context, object, vTables);
}

map<string, Function*> IConcreteObjectType::generateMethodFunctions(IRGenerationContext& context,
                                                                    IConcreteObjectType* object) {
  map<string, Function*> methodFunctionMap;
  vector<tuple<Method*, Function*>> methodsWithFunctions;
  
  for (Method* method : object->getMethods()) {
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

  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTablePortion;
  
  vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
  vTablePortion.push_back(ConstantExpr::getBitCast(typeListGlobal, int8Pointer));
  
  vTables.push_back(vTablePortion);
}

void IConcreteObjectType::addUnthunkInfo(IRGenerationContext& context,
                                         IConcreteObjectType* object,
                                         vector<vector<Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  unsigned long vTableSize = object->getFlattenedInterfaceHierarchy().size() + 1;
  
  for (unsigned long i = 1; i < vTableSize; i++) {
    vector<Constant*> vTablePortion;
    
    long unthunkBy = -Environment::getAddressSizeInBytes() * i;
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTablePortion.push_back(ConstantExpr::getIntToPtr(unthunk, int8Pointer));
    vTablePortion.push_back(ConstantExpr::getNullValue(int8Pointer));
    
    vTables.push_back(vTablePortion);
  }
}

void IConcreteObjectType::generateInterfaceMapFunctions(IRGenerationContext& context,
                                                        IConcreteObjectType* object,
                                                        vector<vector<Constant*>>& vTables) {
  Type* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  map<string, Function*> methodFunctionMap = generateMethodFunctions(context, object);
  
  vector<list<Constant*>> interfaceMapFunctions;
  
  list<Constant*> concreteObjectMethods;
  for (Method* method : object->getMethods()) {
    Function* concreteObjectFunction = methodFunctionMap.at(method->getName());
    Constant* bitCast = ConstantExpr::getBitCast(concreteObjectFunction, int8Pointer);
    concreteObjectMethods.push_back(bitCast);
  }
  interfaceMapFunctions.push_back(concreteObjectMethods);
  
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
  
  assert(interfaceMapFunctions.size() == vTables.size());
  
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

void IConcreteObjectType::declareFieldVariables(IRGenerationContext& context,
                                                IConcreteObjectType* object) {
  map<string, Field*> fields = object->getFields();
  for (map<string, Field*>::const_iterator iterator = fields.begin();
       iterator != fields.end();
       iterator++) {
    Field* field = iterator->second;
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(field->getName(), NULL, object);
    context.getScopes().setVariable(fieldVariable);
  }
}
