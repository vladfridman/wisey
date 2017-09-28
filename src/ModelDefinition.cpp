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
#include "wisey/Log.hpp"
#include "wisey/ModelDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelDefinition::~ModelDefinition() {
  delete mModelTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFieldDeclarations.clear();
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    delete methodDeclaration;
  }
  mMethodDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mModelTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = new Model(fullName, structType);
  context.addModel(model);
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));
  vector<Interface*> interfaces = processInterfaces(context);
  vector<IMethod*> methods = createMethods(context);
  model->setMethods(methods);
  model->setInterfaces(interfaces);

  vector<Field*> fields = createFields(context, model->getInterfaces().size());
  model->setFields(fields);
  
  vector<Type*> types;
  for (Interface* interface : model->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  collectFieldTypes(context, model, types);
  model->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, model);
  IConcreteObjectType::generateVTable(context, model);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));
 
  context.getScopes().pushScope();
  context.getScopes().setObjectType(model);
  
  IConcreteObjectType::generateStaticMethodsIR(context, model);
  IConcreteObjectType::composeDestructorBody(context, model);
  IConcreteObjectType::declareFieldVariables(context, model);
  IConcreteObjectType::generateMethodsIR(context, model);
  
  context.getScopes().popScope(context);

  model->createRTTI(context);
  
  return NULL;
}

vector<Field*> ModelDefinition::createFields(IRGenerationContext& context,
                                             unsigned long numberOfInterfaces) const {
  vector<Field*> fields;
  ExpressionList arguments;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    FieldKind fieldKind = fieldDeclaration->getFieldKind();
    
    if (fieldKind != FIXED_FIELD) {
      Log::e("Models can only have fixed fields");
      exit(1);
    }
    
    Field* field = new Field(fieldKind,
                             fieldType,
                             fieldDeclaration->getName(),
                             numberOfInterfaces + fields.size(),
                             arguments);
    fields.push_back(field);
  }
  return fields;
}

void ModelDefinition::collectFieldTypes(IRGenerationContext& context,
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

vector<IMethod*> ModelDefinition::createMethods(IRGenerationContext& context) const {
  vector<IMethod*> methods;
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    IMethod* method = methodDeclaration->createMethod(context);
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

