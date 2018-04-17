//
//  ControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Controller.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/Environment.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerDefinition::ControllerDefinition(AccessLevel accessLevel,
                                           ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                                           vector<IObjectElementDefinition*>
                                             objectElementDeclarations,
                                           vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                           vector<IObjectDefinition*> innerObjectDefinitions,
                                           int line) :
mAccessLevel(accessLevel),
mControllerTypeSpecifierFull(controllerTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

ControllerDefinition::~ControllerDefinition() {
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

Controller* ControllerDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);

  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Controller* controller = Controller::newController(mAccessLevel, fullName, structType, mLine);
  context.addController(controller);
  controller->setImportProfile(context.getImportProfile());
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjects(context, controller, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return controller;
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  Controller* controller = context.getController(fullName, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, controller, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(lastObjectType);
}

void ControllerDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mControllerTypeSpecifierFull);
  Controller* controller = context.getController(fullName, mLine);
  
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(controller);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, controller);
  IConcreteObjectType::generateStaticMethodsIR(context, controller);
  IConcreteObjectType::declareFieldVariables(context, controller);
  IConcreteObjectType::generateMethodsIR(context, controller);
  IConcreteObjectType::generateLLVMFunctionsIR(context, controller);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, controller);
  IConcreteObjectType::composeInterfaceMapFunctions(context, controller);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
}

