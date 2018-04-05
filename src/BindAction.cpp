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

BindAction::BindAction(IInterfaceTypeSpecifier* interfaceTypeSpecifier,
                       IControllerTypeSpecifier* controllerTypeSpecifier) :
mInterfaceTypeSpecifier(interfaceTypeSpecifier),
mContreollerTypeSpecifier(controllerTypeSpecifier) { }

BindAction::~BindAction() {
  delete mInterfaceTypeSpecifier;
  delete mContreollerTypeSpecifier;
}

IObjectType* BindAction::prototypeObject(IRGenerationContext& context) const {
  return NULL;
}

void BindAction::prototypeMethods(IRGenerationContext& context) const {
  const Interface* interface = (const Interface*) mInterfaceTypeSpecifier->getType(context);
  const Controller* controller = (const Controller*) mContreollerTypeSpecifier->getType(context);
  context.bindInterfaceToController(interface, controller);
}

Value* BindAction::generateIR(IRGenerationContext& context) const {
  return NULL;
}
