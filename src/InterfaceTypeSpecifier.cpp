//
//  InterfaceTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/InterfaceTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

const string InterfaceTypeSpecifier::getName() const {
  return mName;
}

IType* InterfaceTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getInterface(mName);
}
