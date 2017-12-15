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

Assignment::Assignment(Identifier* identifier, IExpression* expression, int line)
: mIdentifier(identifier), mExpression(expression), mLine(line) { }

Assignment::~Assignment() {
  delete mIdentifier;
  delete mExpression;
}

IVariable* Assignment::getVariable(IRGenerationContext& context) const {
  return mIdentifier->getVariable(context);
}

Value* Assignment::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  IVariable* variable = mIdentifier->getVariable(context);
  if (variable == NULL) {
    Log::e("Undeclared variable '" + mIdentifier->getIdentifierName() + "'");
    exit(1);
  }
  
  Value* result = variable->generateAssignmentIR(context, mExpression, mLine);
  
  if (flag == IR_GENERATION_RELEASE) {
    assert(IType::isOwnerType(variable->getType()));
    ((IOwnerVariable*) variable)->setToNull(context);
  }

  return result;
}

const IType* Assignment::getType(IRGenerationContext& context) const {
  return mIdentifier->getType(context);
}

bool Assignment::isConstant() const {
  return false;
}

void Assignment::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mIdentifier->printToStream(context, stream);
  stream << " = ";
  mExpression->printToStream(context, stream);
}
