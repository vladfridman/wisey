//
//  ModelTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IType* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getModel(mName);
}
