//
//  Assignment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/Assignment.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

Assignment::Assignment(IExpression* identifier, IExpression* expression, int line) :
mIdentifier(identifier), mExpression(expression), mLine(line) { }

Assignment::~Assignment() {
  delete mIdentifier;
  delete mExpression;
}

int Assignment::getLine() const {
  return mLine;
}

IVariable* Assignment::getVariable(IRGenerationContext& context,
                                   std::vector<const IExpression*>& arrayIndices) const {
  if (!mIdentifier->isAssignable()) {
    context.reportError(mLine, "Expression is not assignable");
    exit(1);
  }
  return ((IExpressionAssignable*) mIdentifier)->getVariable(context, arrayIndices);
}

Value* Assignment::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  std::vector<const IExpression*> arrayIndices;
  IVariable* variable = getVariable(context, arrayIndices);
  if (variable == NULL) {
    std::stringstream stringStream;
    mIdentifier->printToStream(context, stringStream);
    context.reportError(mLine, "Undeclared variable '" + stringStream.str() + "'");
    exit(1);
  }
  
  Value* result = variable->generateAssignmentIR(context, mExpression, arrayIndices, mLine);
  
  if (assignToType->isOwner()) {
    assert(variable->getType()->isOwner());
    ((IOwnerVariable*) variable)->setToNull(context, mLine);
  }

  return result;
}

const IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier->getType(context);
}

bool Assignment::isConstant() const {
  return false;
}

bool Assignment::isAssignable() const {
  return true;
}

void Assignment::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mIdentifier->printToStream(context, stream);
  stream << " = ";
  mExpression->printToStream(context, stream);
}
