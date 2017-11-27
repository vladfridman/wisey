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
  return context.getScopes().getVariable(mIdentifier->getName());
}

Value* Assignment::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  IVariable* variable = context.getScopes().getVariable(mIdentifier->getName());
  if (variable == NULL) {
    Log::e("Undeclared variable '" + mIdentifier->getName() + "'");
    exit(1);
  }
  
  Composer::pushCallStack(context, mLine);
  
  Value* result = variable->generateAssignmentIR(context, mExpression);
  
  Composer::popCallStack(context);

  return result;
}

const IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier->getType(context);
}

void Assignment::releaseOwnership(IRGenerationContext& context) const {
  mIdentifier->releaseOwnership(context);
}

bool Assignment::isConstant() const {
  return false;
}

void Assignment::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mIdentifier->printToStream(context, stream);
  stream << " = ";
  mExpression->printToStream(context, stream);
}
