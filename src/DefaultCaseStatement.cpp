//
//  DefaultCaseStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/DefaultCaseStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* DefaultCaseStatement::generateIR(IRGenerationContext& context) const {
  return mBlock.generateIR(context);
}

