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
  checkFields(mFieldDeclarations);

  configureConcreteObject(context,
                          model,
                          mFieldDeclarations,
                          mMethodDeclarations,
                          mInterfaceSpecifiers);
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

void ModelDefinition::checkFields(vector<FieldDeclaration*> fieldDeclarations) {
  for (FieldDeclaration* fieldDeclaration : fieldDeclarations) {
    FieldKind fieldKind = fieldDeclaration->getFieldKind();
    
    if (fieldKind != FIXED_FIELD) {
      Log::e("Models can only have fixed fields");
      exit(1);
    }
  }
}
