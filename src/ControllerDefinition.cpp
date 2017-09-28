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
  Controller* controller = new Controller(fullName, structType);
  context.addController(controller);
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifier->getName(context));
  controller->setFields(createFields(context, mInterfaceSpecifiers.size()));

  configureConcreteObject(context,
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

vector<Field*> ControllerDefinition::createFields(IRGenerationContext& context,
                                                  unsigned long startIndex) const {
  vector<Field*> fields;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    FieldKind fieldKind = fieldDeclaration->getFieldKind();

    if (fieldKind != STATE_FIELD && fieldKind != INJECTED_FIELD && fieldKind != RECEIVED_FIELD) {
      Log::e("Controllers can only have fixed, injected or state fields");
      exit(1);
    }
    
    if (fieldKind == INJECTED_FIELD && fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface)->getOwner();
    }

    Field* field = new Field(fieldKind,
                             fieldType,
                             fieldDeclaration->getName(),
                             startIndex + fields.size(),
                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

