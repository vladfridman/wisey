//
//  ExternalModelDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalModelDefinition.hpp"
#include "wisey/ModelDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalModelDefinition::ExternalModelDefinition(ModelTypeSpecifierFull* modelTypeSpecifierFull,
                                                 vector<IObjectElementDefinition*>
                                                   objectElementDeclarations,
                                                 vector<IInterfaceTypeSpecifier*>
                                                 interfaceSpecifiers,
                                                 vector<IObjectDefinition*>
                                                 innerObjectDefinitions,
                                                 bool isPooled,
                                                 int line) :
mModelTypeSpecifierFull(modelTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mIsPooled(isPooled),
mLine(line) { }

ExternalModelDefinition::~ExternalModelDefinition() {
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

Model* ExternalModelDefinition::prototypeObject(IRGenerationContext& context,
                                                ImportProfile* importProfile) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = mIsPooled
  ? Model::newPooledExternalModel(fullName, structType, importProfile, mLine)
  : Model::newExternalModel(fullName, structType, importProfile, mLine);
  context.addModel(model, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjects(context, importProfile, model, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return model;
}

void ExternalModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mModelTypeSpecifierFull);
  Model* model = context.getModel(fullName, mLine);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(model);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers, NULL);
  model->declareBuildFunction(context);
  model->declareRTTI(context);
  context.setObjectType(lastObjectType);
}

void ExternalModelDefinition::generateIR(IRGenerationContext& context) const {
}

