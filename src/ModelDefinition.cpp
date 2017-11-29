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

ModelDefinition::ModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                                 vector<IObjectElementDeclaration*> objectElementDeclarations,
                                 vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
mModelTypeSpecifier(modelTypeSpecifier),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ModelDefinition::~ModelDefinition() {
  delete mModelTypeSpecifier;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mModelTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newModel(fullName, structType);
  context.addModel(model);
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));

  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));
 
  context.getScopes().pushScope();
  context.getScopes().setObjectType(model);
  
  IConcreteObjectType::defineCurrentObjectNameVariable(context, model);
  IConcreteObjectType::generateStaticMethodsIR(context, model);
  IConcreteObjectType::declareFieldVariables(context, model);
  IConcreteObjectType::generateMethodsIR(context, model);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, model);
  model->createRTTI(context);

  context.getScopes().popScope(context, 0);
  
  return NULL;
}

