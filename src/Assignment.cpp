//
//  Assignment.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Assignment.hpp"
#include "yazyk/Log.hpp"

using namespace llvm;
using namespace yazyk;

Value* Assignment::generateIR(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(mIdentifier.getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mIdentifier.getName());
    exit(1);
  }
  
  return variable->generateAssignmentIR(context, mExpression.generateIR(context));
}

IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier.getType(context);
}
