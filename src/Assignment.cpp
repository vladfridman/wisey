//
//  Assignment.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/Assignment.hpp"
#include "yazyk/log.hpp"
#include "yazyk/Identifier.hpp"

using namespace llvm;
using namespace yazyk;

Value* Assignment::generateIR(IRGenerationContext& context) const {
  Variable* variable = context.getScopes().getVariable(mIdentifier.getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mIdentifier.getName());
    exit(1);
  }
  
  return new StoreInst(mExpression.generateIR(context),
                       variable->getValue(),
                       context.getBasicBlock());
}
