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

Value* BindAction::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerId);
  Interface* interface = context.getInterface(mInterfaceId);
  context.bindInterfaceToController(interface, controller);
  
  return NULL;
}
