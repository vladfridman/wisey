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

Assignment::~Assignment() {
  delete mIdentifier;
  delete mExpression;
}

Value* Assignment::generateIR(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariableForAssignement(mIdentifier->getName());
  if (variable == NULL) {
    Log::e("Undeclared variable " + mIdentifier->getName());
    exit(1);
  }
  context.getScopes().eraseFromClearedVariables(variable);
  
  const IType* identifierType = getType(context);
  TypeKind identifierTypeKind = identifierType->getTypeKind();
  if (identifierTypeKind == CONTROLLER_TYPE || identifierTypeKind == CONTROLLER_OWNER_TYPE) {
    Log::e("Can not assign to controllers");
    exit(1);
  }
  
  Value* result = variable->generateAssignmentIR(context, mExpression);
  
  if (IType::isOwnerType(identifierType)) {
    mExpression->releaseOwnership(context);
  }
  
  return result;
}

const IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier->getType(context);
}

void Assignment::releaseOwnership(IRGenerationContext& context) const {
  mIdentifier->releaseOwnership(context);
}

bool Assignment::existsInOuterScope(IRGenerationContext& context) const {
  return mIdentifier->existsInOuterScope(context);
}
