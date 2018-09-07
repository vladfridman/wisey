//
//  ReturnStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReturnStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace wisey;

ReturnStatement::ReturnStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

ReturnStatement::~ReturnStatement() {
  delete mExpression;
}

void ReturnStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  IExpression::checkForUndefined(context, mExpression);

  const IType* returnType = context.getScopes().getReturnType();
  if (returnType == NULL) {
    context.reportError(mLine, "No corresponding method found for RETURN");
    throw 1;
  }
  if (returnType == PrimitiveTypes::VOID) {
    context.reportError(mLine, "Can't return value of type void");
    throw 1;
  }

  if (context.getScopes().hasOwnerVariables()) {
    Composer::setLineNumber(context, mLine);
  }

  Value* result = AutoCast::maybeCast(context,
                                      mExpression->getType(context),
                                      mExpression->generateIR(context, returnType),
                                      returnType,
                                      mLine);
  if (returnType->isReference() && !returnType->isNative()) {
    ((const IReferenceType*) returnType)->incrementReferenceCount(context, result);
  }
  
  context.getScopes().freeOwnedMemory(context, NULL, mLine);
  
  if (returnType->isReference() && !returnType->isNative()) {
    ((const IReferenceType*) returnType)->decrementReferenceCount(context, result);
  }

  Composer::popCallStack(context);
  IRWriter::createReturnInst(context, result);
}

