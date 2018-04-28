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
                                                           vector<IObjectElementDefinition*>
                                                             objectElementDeclarations,
                                                           vector<IInterfaceTypeSpecifier*>
                                                           interfaceSpecifiers,
                                                           vector<IObjectDefinition*>
                                                           innerObjectDefinitions,
                                                           int line) :
mControllerTypeSpecifierFull(controllerTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

ExternalControllerDefinition::~ExternalControllerDefinition() {
  delete mControllerTypeSpecifierFull;
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

Controller* ExternalControllerDefinition::prototypeObject(IRGenerationContext&
                                                                context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);

  Controller* controller = Controller::newExternalController(fullName,
                                                             structType,
                                                             context.getImportProfile(),
                                                             mLine);
  context.addController(controller);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjects(context, controller, mInnerObjectDefinitions);
  controller->declareInjectFunction(context, mLine);
  context.setObjectType(lastObjectType);
  
  return controller;
}

void ExternalControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  Controller* controller = context.getController(fullName, mLine);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, controller, mObjectElementDeclarations, mInterfaceSpecifiers);
  controller->declareFieldInjectionFunctions(context, mLine);
  context.setObjectType(lastObjectType);
}

void ExternalControllerDefinition::generateIR(IRGenerationContext& context) const {
}
