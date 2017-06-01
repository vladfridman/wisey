//
//  Assignment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Assignment.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

Value* Assignment::generateIR(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(mIdentifier.getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mIdentifier.getName());
    exit(1);
  }
  
  if (variable->getType()->getTypeKind() == CONTROLLER_TYPE) {
    Log::e("Can not assign to controllers");
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
