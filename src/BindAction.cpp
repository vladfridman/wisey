//
//  BindAction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "BindAction.hpp"

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

const IInterfaceTypeSpecifier* BindAction::getInterface() const {
  return mInterfaceTypeSpecifier;
}

const IControllerTypeSpecifier* BindAction::getController() const {
  return mContreollerTypeSpecifier;
}

InjectionArgumentList BindAction::getInjectionArguments() const {
  return mInjectionArguments;
}
