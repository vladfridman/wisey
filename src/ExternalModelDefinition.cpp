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

ExternalModelDefinition::ExternalModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                                                 vector<IObjectElementDeclaration*>
                                                   objectElementDeclarations,
                                                 vector<InterfaceTypeSpecifier*>
                                                 interfaceSpecifiers) :
mModelTypeSpecifier(modelTypeSpecifier),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ExternalModelDefinition::~ExternalModelDefinition() {
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

void ExternalModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mModelTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = Model::newExternalModel(fullName, structType);
  context.addModel(model);
}

void ExternalModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));
  
  configureObject(context, model, mObjectElementDeclarations, mInterfaceSpecifiers);
}

Value* ExternalModelDefinition::generateIR(IRGenerationContext& context) const {
  Model* model = context.getModel(mModelTypeSpecifier->getName(context));
  model->createRTTI(context);
  
  return NULL;
}

