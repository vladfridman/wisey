//
//  Assignment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Assignment.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

Assignment::~Assignment() {
  delete mIdentifier;
  delete mExpression;
}

IVariable* Assignment::getVariable(IRGenerationContext& context) const {
  return context.getScopes().getVariableForAssignement(mIdentifier->getName());
}

Value* Assignment::generateIR(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariableForAssignement(mIdentifier->getName());
  if (variable == NULL) {
    Log::e("Undeclared variable '" + mIdentifier->getName() + "'");
    exit(1);
  }
  
  const IType* identifierType = getType(context);
  
  Composer::pushCallStack(context, mLine);
  
  Value* result = variable->generateAssignmentIR(context, mExpression);
  
  if (IType::isOwnerType(identifierType)) {
    mExpression->releaseOwnership(context);
  }
  
  Composer::popCallStack(context);

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

bool Assignment::isConstant() const {
  return false;
}

void Assignment::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mIdentifier->printToStream(context, stream);
  stream << " = ";
  mExpression->printToStream(context, stream);
}
