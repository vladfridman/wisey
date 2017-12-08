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
                                                 vector<IObjectElementDeclaration*>
                                                   objectElementDeclarations,
                                                 vector<IInterfaceTypeSpecifier*>
                                                 interfaceSpecifiers,
                                                 vector<IObjectDefinition*>
                                                 innerObjectDefinitions) :
mModelTypeSpecifierFull(modelTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

ExternalModelDefinition::~ExternalModelDefinition() {
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

void ExternalModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mModelTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newExternalModel(fullName, structType);
  context.addModel(model);
  model->setImportProfile(context.getImportProfile());
}

void ExternalModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifierFull->getName(context));
  
  context.setObjectType(model);
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(NULL);
}

Value* ExternalModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifierFull->getName(context));
  model->createRTTI(context);
  
  return NULL;
}

