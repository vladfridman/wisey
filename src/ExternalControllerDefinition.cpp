//
//  ExternalControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ControllerDefinition.hpp"
#include "ExternalControllerDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalControllerDefinition::
ExternalControllerDefinition(ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                             vector<IObjectElementDefinition*> objectElementDeclarations,
                             vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                             vector<IObjectDefinition*> innerObjectDefinitions,
                             const IObjectTypeSpecifier* scopeTypeSpecifier,
                             int line) :
mControllerTypeSpecifierFull(controllerTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mScopeTypeSpecifier(scopeTypeSpecifier),
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
  delete mScopeTypeSpecifier;
}

Controller* ExternalControllerDefinition::prototypeObject(IRGenerationContext& context,
                                                          ImportProfile* importProfile) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);

  Controller* controller = mScopeTypeSpecifier == NULL
  ? Controller::newExternalController(fullName, structType, importProfile, mLine)
  : Controller::newExternalScopedController(fullName, structType, importProfile, mLine);
  
  context.addController(controller, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjects(context,
                                           importProfile,
                                           controller,
                                           mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return controller;
}

void ExternalControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  Controller* controller = context.getController(fullName, mLine);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context,
                  controller,
                  mObjectElementDeclarations,
                  mInterfaceSpecifiers,
                  mScopeTypeSpecifier);
  controller->declareInjectFunction(context, mLine);
  context.setObjectType(lastObjectType);
}

void ExternalControllerDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  Controller* controller = context.getController(fullName, mLine);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  controller->declareFieldInjectionFunctions(context, mLine);
  context.setObjectType(lastObjectType);
}
