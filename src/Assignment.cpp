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
  
  Value* result = variable->generateAssignmentIR(context, mExpression);
  
  if (getType(context)->getTypeKind() != PRIMITIVE_TYPE) {
    mExpression.releaseOwnership(context);
  }
  
  return result;
}

IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier.getType(context);
}

void Assignment::releaseOwnership(IRGenerationContext& context) const {
  mIdentifier.releaseOwnership(context);
}
