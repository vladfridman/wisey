//
//  ControllerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

const string ControllerTypeSpecifier::getName() const {
  return mName;
}

IType* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getController(mName);
}

