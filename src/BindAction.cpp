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
                       IControllerTypeSpecifier* controllerTypeSpecifier,
                       InjectionArgumentList injectionArguments) :
mInterfaceTypeSpecifier(interfaceTypeSpecifier),
mContreollerTypeSpecifier(controllerTypeSpecifier),
mInjectionArguments(injectionArguments) { }

BindAction::~BindAction() {
  delete mInterfaceTypeSpecifier;
  delete mContreollerTypeSpecifier;
  for (InjectionArgument* argument : mInjectionArguments) {
    delete argument;
  }
  mInjectionArguments.clear();
}

const Interface* BindAction::getInterface(IRGenerationContext& context) const {
  return mInterfaceTypeSpecifier->getType(context);
}

const Controller* BindAction::getController(IRGenerationContext& context) const {
  return mContreollerTypeSpecifier->getType(context);
}

InjectionArgumentList BindAction::getInjectionArguments(IRGenerationContext& context) const {
  return mInjectionArguments;
}
