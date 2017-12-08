//
//  BindAction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/BindAction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BindAction::BindAction(IControllerTypeSpecifier* controllerTypeSpecifier,
                       IInterfaceTypeSpecifier* interfaceTypeSpecifier) :
mContreollerTypeSpecifier(controllerTypeSpecifier),
mInterfaceTypeSpecifier(interfaceTypeSpecifier) { }

BindAction::~BindAction() {
  delete mContreollerTypeSpecifier;
  delete mInterfaceTypeSpecifier;
}

const IObjectType* BindAction::prototypeObject(IRGenerationContext& context) const {
  return NULL;
}

void BindAction::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = (Controller*) mContreollerTypeSpecifier->getType(context);
  Interface* interface = (Interface*) mInterfaceTypeSpecifier->getType(context);
  context.bindInterfaceToController(interface, controller);
}

Value* BindAction::generateIR(IRGenerationContext& context) const {
  return NULL;
}
