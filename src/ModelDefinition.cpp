//
//  ModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/DerivedTypes.h>

#include "yazyk/Environment.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/ObjectFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ModelDefinition::~ModelDefinition() {
  mFieldDeclarations.clear();
  mMethodDeclarations.clear();
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  StructType *structType = StructType::create(llvmContext, "model." + mName);
  
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  map<string, Field*> fields = createFields(context, interfaces.size());
  vector<Method*> methods = createMethods(context);
  Model* model = new Model(mName, structType, fields, methods, interfaces);

  context.getScopes().pushScope();

  createFieldVariables(context, model, types);
  structType->setBody(types);
  map<string, Function*> methodFunctionMap = generateMethodsIR(context, model);
  defineModelTypeName(context, model);
  GlobalVariable* typeListGlobal = createTypeListGlobal(context, model);
  processInterfaceMethods(context, model, interfaces, methodFunctionMap, typeListGlobal);

  context.addModel(model);
  context.getScopes().popScope(context);

  return NULL;
}

map<string, Field*> ModelDefinition::createFields(IRGenerationContext& context,
                                                  unsigned long numberOfInterfaces) const {
  map<string, Field*> fields;
  unsigned long index = 0;
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    Field* field = new Field(fieldType, fieldDeclaration->getName(), numberOfInterfaces + index);
    fields[fieldDeclaration->getName()] = field;
    index++;
  }
  return fields;
}

void ModelDefinition::createFieldVariables(IRGenerationContext& context,
                                           Model* model,
                                           vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    types.push_back(fieldType->getLLVMType(llvmContext));
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(fieldDeclaration->getName(),
                                                                 NULL,
                                                                 model);
    context.getScopes().setVariable(fieldVariable);
  }
}

vector<Method*> ModelDefinition::createMethods(IRGenerationContext& context) const {
  vector<Method*> methods;
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = methodDeclaration->createMethod(context, methods.size());
    methods.push_back(method);
  }
  return methods;
}

map<string, Function*> ModelDefinition::generateMethodsIR(IRGenerationContext& context,
                                                          Model* model) const {
  map<string, Function*> methodFunctionMap;
  
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = model->findMethod(methodDeclaration->getMethodName());
    Function* function = method->generateIR(context, model);
    methodFunctionMap[method->getName()] = function;
  }
  return methodFunctionMap;
}

std::vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (string interfaceName : mInterfaces) {
    Interface* interface = context.getInterface(interfaceName);
    types.push_back(interface->getLLVMType(context.getLLVMContext())->getPointerElementType());
    interfaces.push_back(interface);
  }
  return interfaces;
}

void ModelDefinition::processInterfaceMethods(IRGenerationContext& context,
                                              Model* model,
                                              vector<Interface*> interfaces,
                                              map<string, Function*>& methodFunctionMap,
                                              GlobalVariable* typeListGlobal) const {

  vector<list<Constant*>> interfaceMapFunctions =
    generateInterfaceMapFunctions(context, model, interfaces, methodFunctionMap);

  vector<vector<Constant*>> interfaceVTables =
    addUnthunkAndTypeTableInfo(context, model, typeListGlobal, interfaceMapFunctions);
  
  createVTableGlobal(context, model, interfaceVTables);
}

vector<list<Constant*>> ModelDefinition::
generateInterfaceMapFunctions(IRGenerationContext& context,
                              Model* model,
                              vector<Interface*> interfaces,
                              map<string, Function*>& methodFunctionMap) const {
  
  vector<list<Constant*>> interfaceMapFunctions;
  for (Interface* interface : interfaces) {
    vector<list<Constant*>> vSubTable =
    interface->generateMapFunctionsIR(context,
                                      model,
                                      methodFunctionMap,
                                      interfaceMapFunctions.size());
    for (list<Constant*> vTablePortion : vSubTable) {
      interfaceMapFunctions.push_back(vTablePortion);
    }
  }
  
  return interfaceMapFunctions;
}

vector<vector<Constant*>> ModelDefinition::
addUnthunkAndTypeTableInfo(IRGenerationContext& context,
                           Model* model,
                           GlobalVariable* typeListGlobal,
                           vector<list<Constant*>> interfaceMapFunctions) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();

  vector<vector<Constant*>> interfaceVTables;
  for (list<Constant*> vTablePortion : interfaceMapFunctions) {
    if (interfaceVTables.size() == 0) {
      Constant* bitCast = ConstantExpr::getBitCast(typeListGlobal, pointerType);
      vTablePortion.push_front(bitCast);
      vTablePortion.push_front(ConstantExpr::getNullValue(pointerType));
    } else {
      vTablePortion.push_front(ConstantExpr::getNullValue(pointerType));
      long unthunkBy = -Environment::getAddressSizeInBytes() * ((long) interfaceVTables.size());
      ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
      vTablePortion.push_front(ConstantExpr::getIntToPtr(unthunk, pointerType));
    }
    
    vector<Constant*> vTablePortionVector {
      std::make_move_iterator(std::begin(vTablePortion)),
      std::make_move_iterator(std::end(vTablePortion))
    };
    
    interfaceVTables.push_back(vTablePortionVector);
  }
  
  return interfaceVTables;
}

void ModelDefinition::createVTableGlobal(IRGenerationContext& context,
                                         Model* model,
                                         vector<vector<Constant*>> interfaceVTables) const {
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
                     model->getVTableName());
}

void ModelDefinition::defineModelTypeName(IRGenerationContext& context, Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, model->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     model->getObjectNameGlobalVariableName());
}

GlobalVariable* ModelDefinition::createTypeListGlobal(IRGenerationContext& context,
                                                      Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = model->getFlattenedInterfaceHierarchy();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();

  GlobalVariable* modelNameGlobal =
    context.getModule()->getGlobalVariable(model->getObjectNameGlobalVariableName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = modelNameGlobal->getType()->getPointerElementType();
  Constant* modelNamePointer = ConstantExpr::getGetElementPtr(elementType, modelNameGlobal, Idx);
  
  vector<Constant*> typeNames;
  typeNames.push_back(modelNamePointer);
  
  for (Interface* interface : interfaces) {
    GlobalVariable* interfaceNameGlobal =
      context.getModule()->getGlobalVariable(interface->getObjectNameGlobalVariableName());
    Type* elementType = interfaceNameGlobal->getType()->getPointerElementType();
    Constant* interfaceNamePointer =
      ConstantExpr::getGetElementPtr(elementType, interfaceNameGlobal, Idx);
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
                            model->getTypeTableName());
}
