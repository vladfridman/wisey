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
  map<string, ModelField*> fields = createFields(context, interfaces.size());
  vector<Method*> methods = createMethods(context);
  Model* model = new Model(mName, structType, fields, methods, interfaces);

  context.getScopes().pushScope();

  createFieldVariables(context, model, types);
  structType->setBody(types);
  map<string, Function*> methodFunctionMap = generateMethodsIR(context, model);
  processInterfaceMethods(context, model, interfaces, methodFunctionMap);
  defineModelTypeName(context, model);
  addTypeInformation(context, model);

  context.addModel(model);
  context.getScopes().popScope(context);

  return NULL;
}

map<string, ModelField*> ModelDefinition::createFields(IRGenerationContext& context,
                                                       unsigned long numberOfInterfaces) const {
  map<string, ModelField*> fields;
  unsigned long index = 0;
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    ModelField* modelField = new ModelField(fieldType, numberOfInterfaces + index);
    fields[fieldDeclaration->getName()] = modelField;
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
                                              map<string, Function*>& methodFunctionMap) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Constant*> vTableArray;
  vector<Type*> vTableTypes;
  for (Interface* interface : interfaces) {
    vector<vector<Constant*>> vSubTable =
      interface->generateMapFunctionsIR(context, model, methodFunctionMap, vTableArray.size());
    for (vector<Constant*> vTablePortion : vSubTable) {
      ArrayRef<Constant*> arrayRef(vTablePortion);
      Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
      ArrayType* arrayType = ArrayType::get(pointerType, vTablePortion.size());
      Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
      
      vTableArray.push_back(constantArray);
      vTableTypes.push_back(arrayType);
    }
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
                     model->getModelNameVariableName());
}

void ModelDefinition::addTypeInformation(IRGenerationContext& context, Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = model->getFlattenedInterfaceHierarchy();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();

  GlobalVariable* modelNamePointer =
    context.getModule()->getGlobalVariable(model->getModelNameVariableName());
  Constant* bitCast = ConstantExpr::getBitCast(modelNamePointer, pointerType);

  vector<Constant*> typeNames;
  typeNames.push_back(bitCast);
  
  for (Interface* interface : interfaces) {
    GlobalVariable* interfaceNamePointer =
      context.getModule()->getGlobalVariable(interface->getInterfaceNameVariableName());
    Constant* bitCast = ConstantExpr::getBitCast(interfaceNamePointer, pointerType);
    typeNames.push_back(bitCast);
  }
  ArrayRef<Constant*> arrayRef(typeNames);
  ArrayType* arrayType = ArrayType::get(pointerType, typeNames.size());
  Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
 
  new GlobalVariable(*context.getModule(),
                     arrayType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     constantArray,
                     model->getTypeTableName());
}
