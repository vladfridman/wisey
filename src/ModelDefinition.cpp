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
#include "wisey/ModelDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelDefinition::~ModelDefinition() {
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFieldDeclarations.clear();
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    delete methodDeclaration;
  }
  mMethodDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = new Model(fullName, structType);
  context.addModel(model);
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(getFullName(context));
  vector<Interface*> interfaces = processInterfaces(context);
  vector<Method*> methods = createMethods(context);
  model->setMethods(methods);
  model->setInterfaces(interfaces);

  map<string, Field*> fields = createFields(context, model->getInterfaces().size());
  model->setFields(fields);
  
  vector<Type*> types;
  for (Interface* interface : model->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  createFieldVariables(context, model, types);
  model->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, model);
  IConcreteObjectType::generateVTable(context, model);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(getFullName(context));
 
  context.getScopes().pushScope();
  
  IConcreteObjectType::declareFieldVariables(context, model);

  for (Method* method : model->getMethods()) {
    method->generateIR(context, model);
  }
  
  context.getScopes().popScope(context);

  model->createRTTI(context);
  
  return NULL;
}

map<string, Field*> ModelDefinition::createFields(IRGenerationContext& context,
                                                  unsigned long numberOfInterfaces) const {
  map<string, Field*> fields;
  ExpressionList arguments;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
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
  
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    Type* llvmType = fieldType->getLLVMType(llvmContext);
    if (IType::isReferenceType(fieldType)) {
      types.push_back(llvmType->getPointerElementType());
    } else {
      types.push_back(llvmType);
    }
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

vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    interfaces.push_back(interface);
  }
  return interfaces;
}

string ModelDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}
