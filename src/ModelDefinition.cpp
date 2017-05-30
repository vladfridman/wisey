//
//  ModelDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/Environment.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/ObjectFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelDefinition::~ModelDefinition() {
  mFieldDeclarations.clear();
  mMethodDeclarations.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = new Model(fullName, structType);
  context.addModel(model);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  map<string, Field*> fields = createFields(context, interfaces.size());
  vector<Method*> methods = createMethods(context);
  Model* model = context.getModel(getFullName(context));
  model->setFields(fields);
  model->setMethods(methods);
  model->setInterfaces(interfaces);

  context.getScopes().pushScope();

  createFieldVariables(context, model, types);
  model->setStructBodyTypes(types);
  map<string, Function*> methodFunctionMap = generateMethodsIR(context, model);
  defineTypeName(context, model);
  
  IConcreteObjectType::generateVTable(context, model, methodFunctionMap);
  
  context.addImport(model);
  context.getScopes().popScope(context);

  model->createRTTI(context);
  
  return NULL;
}

map<string, Field*> ModelDefinition::createFields(IRGenerationContext& context,
                                                  unsigned long numberOfInterfaces) const {
  map<string, Field*> fields;
  ExpressionList arguments;
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    Field* field = new Field(fieldType,
                             fieldDeclaration->getName(),
                             numberOfInterfaces + fields.size(),
                             arguments);
    fields[fieldDeclaration->getName()] = field;
  }
  return fields;
}

void ModelDefinition::createFieldVariables(IRGenerationContext& context,
                                           Model* model,
                                           vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
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
  vector<tuple<Method*, Function*>> methodsWithFunctions;
  
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = model->findMethod(methodDeclaration->getMethodName());
    Function* function = method->defineFunction(context, model);
    methodFunctionMap[method->getName()] = function;
    methodsWithFunctions.push_back(make_tuple(method, function));
  }
  
  for (tuple<Method*, Function*> methodAndFunction : methodsWithFunctions) {
    Method* method = get<0>(methodAndFunction);
    Function* function = get<1>(methodAndFunction);
    method->generateIR(context, function, model);
  }
  
  return methodFunctionMap;
}

std::vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    types.push_back(interface->getLLVMType(context.getLLVMContext())->getPointerElementType());
    interfaces.push_back(interface);
  }
  return interfaces;
}

void ModelDefinition::defineTypeName(IRGenerationContext& context, Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, model->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     model->getObjectNameGlobalVariableName());
}

string ModelDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}
