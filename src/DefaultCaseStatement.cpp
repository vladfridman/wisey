//
//  DefaultCaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/DefaultCaseStatement.hpp"

using namespace llvm;
using namespace wisey;

void DefaultCaseStatement::prototypeObjects(IRGenerationContext& context) const {
}

Value* DefaultCaseStatement::generateIR(IRGenerationContext& context) const {
  return mBlock.generateIR(context);
}

