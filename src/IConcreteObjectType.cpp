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

void IConcreteObjectType::generateVTable(IRGenerationContext& context,
                                         IConcreteObjectType* object,
                                         map<string, Function*>& methodFunctionMap) {
  vector<vector<Constant*>> vTables;

  addTypeListInfo(context, object, vTables);
  addUnthunkInfo(context, object, vTables);
  generateInterfaceMapFunctions(context, object, vTables, methodFunctionMap);
  createVTableGlobal(context, object, vTables);
}

void IConcreteObjectType::addTypeListInfo(IRGenerationContext& context,
                                        IConcreteObjectType* object,
                                        vector<vector<Constant*>>& vTables) {
  GlobalVariable* typeListGlobal = createTypeListGlobal(context, object);

  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTablePortion;
  
  Constant* bitCast = ConstantExpr::getBitCast(typeListGlobal, pointerType);
  vTablePortion.push_back(ConstantExpr::getNullValue(pointerType));
  vTablePortion.push_back(bitCast);
  
  vTables.push_back(vTablePortion);
}

void IConcreteObjectType::addUnthunkInfo(IRGenerationContext& context,
                                         IConcreteObjectType* object,
                                         vector<vector<Constant*>>& vTables) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  unsigned long vTableSize = object->getFlattenedInterfaceHierarchy().size();
  
  for (unsigned long i = 1; i < vTableSize; i++) {
    vector<Constant*> vTablePortion;
    
    long unthunkBy = -Environment::getAddressSizeInBytes() * i;
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTablePortion.push_back(ConstantExpr::getIntToPtr(unthunk, pointerType));
    vTablePortion.push_back(ConstantExpr::getNullValue(pointerType));
    
    vTables.push_back(vTablePortion);
  }
}

void IConcreteObjectType::generateInterfaceMapFunctions(IRGenerationContext& context,
                                                        IConcreteObjectType* object,
                                                        vector<vector<Constant*>>& vTables,
                                                        map<string, Function*>&
                                                        methodFunctionMap) {
  
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
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Constant*> vTableArray;
  vector<Type*> vTableTypes;
  for (vector<Constant*> vTablePortionVector : interfaceVTables) {
    ArrayRef<Constant*> arrayRef(vTablePortionVector);
    ArrayType* arrayType = ArrayType::get(pointerType, vTablePortionVector.size());
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
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  Constant* controllerNamePointer = IObjectType::getObjectNamePointer(object, context);
  
  vector<Constant*> typeNames;
  typeNames.push_back(controllerNamePointer);
  
  for (Interface* interface : interfaces) {
    Constant* interfaceNamePointer = IObjectType::getObjectNamePointer(interface, context);
    typeNames.push_back(interfaceNamePointer);
  }
  typeNames.push_back(ConstantExpr::getNullValue(pointerType));
  ArrayRef<Constant*> arrayRef(typeNames);
  ArrayType* arrayType = ArrayType::get(pointerType, typeNames.size());
  Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
  
  return new GlobalVariable(*context.getModule(),
                            arrayType,
                            true,
                            GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                            constantArray,
                            object->getTypeTableName());
}
