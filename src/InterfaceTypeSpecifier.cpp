//
//  InterfaceTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

const string InterfaceTypeSpecifier::getName() const {
  return mName;
}

IType* InterfaceTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getInterface(mName);
}
