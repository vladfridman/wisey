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

ControllerDefinition::ControllerDefinition(ControllerTypeSpecifierFull* controllerTypeSpecifierFull,
                                           vector<IObjectElementDeclaration*>
                                             objectElementDeclarations,
                                           vector<IInterfaceTypeSpecifier*> interfaceSpecifiers) :
mControllerTypeSpecifierFull(controllerTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ControllerDefinition::~ControllerDefinition() {
  delete mControllerTypeSpecifierFull;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mControllerTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);

  Controller* controller = Controller::newController(fullName, structType);
  context.addController(controller);
  controller->setImportProfile(context.getImportProfile());
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifierFull->getName(context));

  context.setObjectType(controller);
  configureObject(context, controller, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(NULL);
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifierFull->getName(context));
  
  context.getScopes().pushScope();
  context.setObjectType(controller);
 
  IConcreteObjectType::defineCurrentObjectNameVariable(context, controller);
  IConcreteObjectType::generateStaticMethodsIR(context, controller);
  IConcreteObjectType::declareFieldVariables(context, controller);
  IConcreteObjectType::generateMethodsIR(context, controller);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, controller);
  
  context.setObjectType(NULL);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

