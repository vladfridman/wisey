//
//  BindAction.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/BindAction.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* BindAction::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerId);
  Interface* interface = context.getInterface(mInterfaceId);
  context.bindInterfaceToController(interface, controller);
  
  return NULL;
}
