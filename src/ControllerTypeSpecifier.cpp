//
//  ControllerTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ControllerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

const string ControllerTypeSpecifier::getName() const {
  return mName;
}

IType* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getController(mName);
}

