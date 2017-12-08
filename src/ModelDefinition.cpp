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
                                 vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                 vector<IObjectDefinition*> innerObjectDefinitions) :
mModelTypeSpecifierFull(modelTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

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
  for (IObjectDefinition* innerObjectDefinition : mInnerObjectDefinitions) {
    delete innerObjectDefinition;
  }
  mInnerObjectDefinitions.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newModel(fullName, structType);
  context.addModel(model);
  model->setImportProfile(context.getImportProfile());

  const IObjectType* lastObjectType = context.getObjectType();
  if (lastObjectType) {
    lastObjectType->getImportProfile()->addImport(mModelTypeSpecifierFull->getShortName(),
                                                  fullName);
  }
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjects(context, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  Model* model = context.getModel(fullName);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
  model->createRTTI(context);
  context.setObjectType(lastObjectType);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  Model* model = context.getModel(fullName);
 
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, model);
  IConcreteObjectType::generateStaticMethodsIR(context, model);
  IConcreteObjectType::declareFieldVariables(context, model);
  IConcreteObjectType::generateMethodsIR(context, model);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, model);
  IConcreteObjectType::composeInterfaceMapFunctions(context, model);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

