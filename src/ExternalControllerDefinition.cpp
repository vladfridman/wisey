//
//  ExternalControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerDefinition.hpp"
#include "wisey/ExternalControllerDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalControllerDefinition::ExternalControllerDefinition(ControllerTypeSpecifierFull*
                                                           controllerTypeSpecifierFull,
                                                           vector<IObjectElementDeclaration*>
                                                             objectElementDeclarations,
                                                           vector<IInterfaceTypeSpecifier*>
                                                           interfaceSpecifiers,
                                                           vector<IObjectDefinition*>
                                                           innerObjectDefinitions) :
mControllerTypeSpecifierFull(controllerTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

ExternalControllerDefinition::~ExternalControllerDefinition() {
  delete mControllerTypeSpecifierFull;
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

void ExternalControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mControllerTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);

  Controller* controller = Controller::newExternalController(fullName, structType);
  context.addController(controller);
  controller->setImportProfile(context.getImportProfile());
}

void ExternalControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifierFull->getName(context));
  
  context.setObjectType(controller);
  configureObject(context, controller, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(NULL);
}

Value* ExternalControllerDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}
