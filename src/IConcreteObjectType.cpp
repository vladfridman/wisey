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

#include "wisey/Environment.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"

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

void IConcreteObjectType::generateVTable(IRGenerationContext& context,
                                         IConcreteObjectType* object) {

  vector<vector<Constant*>> vTables;

  addTypeListInfo(context, object, vTables);
  addUnthunkInfo(context, object, vTables);
  generateInterfaceMapFunctions(context, object, vTables);
  createVTableGlobal(context, object, vTables);
}

map<string, Function*> IConcreteObjectType::generateMethodsIR(IRGenerationContext& context,
                                                              IConcreteObjectType* object) {
  map<string, Function*> methodFunctionMap;
  vector<tuple<Method*, Function*>> methodsWithFunctions;
  
  for (Method* method : object->getMethods()) {
    Function* function = method->defineFunction(context, object);
    methodFunctionMap[method->getName()] = function;
    methodsWithFunctions.push_back(make_tuple(method, function));
  }
  
  for (tuple<Method*, Function*> methodAndFunction : methodsWithFunctions) {
    Method* method = get<0>(methodAndFunction);
    Function* function = get<1>(methodAndFunction);
    method->generateIR(context, function, object);
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
  map<string, Function*> methodFunctionMap = generateMethodsIR(context, object);
  
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
