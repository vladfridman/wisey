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

ModelDefinition::ModelDefinition(ModelTypeSpecifierFull* modelTypeSpecifierFull,
                                 vector<IObjectElementDeclaration*> objectElementDeclarations,
                                 vector<IInterfaceTypeSpecifier*> interfaceSpecifiers) :
mModelTypeSpecifierFull(modelTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ModelDefinition::~ModelDefinition() {
  delete mModelTypeSpecifierFull;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mModelTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newModel(fullName, structType);
  context.addModel(model);
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifierFull->getName(context));

  context.setObjectType(model);
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
  model->createRTTI(context);
  context.setObjectType(NULL);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifierFull->getName(context));
 
  context.getScopes().pushScope();
  context.setObjectType(model);
  
  IConcreteObjectType::defineCurrentObjectNameVariable(context, model);
  IConcreteObjectType::generateStaticMethodsIR(context, model);
  IConcreteObjectType::declareFieldVariables(context, model);
  IConcreteObjectType::generateMethodsIR(context, model);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, model);

  context.setObjectType(NULL);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

