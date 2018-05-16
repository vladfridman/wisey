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

ModelDefinition::ModelDefinition(AccessLevel accessLevel,
                                 ModelTypeSpecifierFull* modelTypeSpecifierFull,
                                 vector<IObjectElementDefinition*> objectElementDeclarations,
                                 vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                 vector<IObjectDefinition*> innerObjectDefinitions,
                                 int line) :
mAccessLevel(accessLevel),
mModelTypeSpecifierFull(modelTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

ModelDefinition::~ModelDefinition() {
  delete mModelTypeSpecifierFull;
  for (IObjectElementDefinition* objectElementDeclaration : mObjectElementDeclarations) {
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

Model* ModelDefinition::prototypeObject(IRGenerationContext& context,
                                        ImportProfile* importProfile) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newModel(mAccessLevel, fullName, structType, importProfile, mLine);
  context.addModel(model, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjects(context, importProfile, model, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return model;
}

void ModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  Model* model = context.getModel(fullName, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
  model->defineRTTI(context);
  model->defineBuildFunction(context);
  context.setObjectType(lastObjectType);
}

void ModelDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  Model* model = context.getModel(fullName, mLine);
 
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, model);
  IConcreteObjectType::generateStaticMethodsIR(context, model);
  IConcreteObjectType::declareFieldVariables(context, model);
  IConcreteObjectType::generateMethodsIR(context, model);
  IConcreteObjectType::generateLLVMFunctionsIR(context, model);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, model);
  IConcreteObjectType::composeInterfaceMapFunctions(context, model);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
}

