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

ControllerDefinition::~ControllerDefinition() {
  delete mControllerTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFieldDeclarations.clear();
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    delete methodDeclaration;
  }
  mMethodDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mControllerTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);

  Controller* controller = Controller::newController(fullName, structType);
  context.addController(controller);
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifier->getName(context));
  checkFields(mFieldDeclarations);

  configureObject(context,
                  controller,
                  mFieldDeclarations,
                  mMethodDeclarations,
                  mInterfaceSpecifiers);
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifier->getName(context));
  
  context.getScopes().pushScope();
  context.getScopes().setObjectType(controller);
 
  IConcreteObjectType::generateStaticMethodsIR(context, controller);
  IConcreteObjectType::declareFieldVariables(context, controller);
  IConcreteObjectType::composeDestructorBody(context, controller);
  IConcreteObjectType::generateMethodsIR(context, controller);
  
  context.getScopes().popScope(context);
  
  return NULL;
}

void ControllerDefinition::checkFields(vector<FieldDeclaration*> fieldDeclarations) {
  for (FieldDeclaration* fieldDeclaration : fieldDeclarations) {
    FieldKind fieldKind = fieldDeclaration->getFieldKind();
    
    if (fieldKind != STATE_FIELD && fieldKind != INJECTED_FIELD && fieldKind != RECEIVED_FIELD) {
      Log::e("Controllers can only have fixed, injected or state fields");
      exit(1);
    }
  }
}
