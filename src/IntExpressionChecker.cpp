//
//  IntExpressionChecker.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IntExpressionChecker.hpp"
#include "wisey/UndefinedType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IntExpressionChecker::checkTypes(IRGenerationContext& context,
                                      const IBinaryExpression* expression) {
  const IExpression* leftExpression = expression->getLeft();
  const IExpression* rightExpression = expression->getRight();

  IExpression::checkForUndefined(context, leftExpression);
  IExpression::checkForUndefined(context, rightExpression);

  Type* leftLLVMType = leftExpression->getType(context)->getLLVMType(context);
  Type* rightLLVMType = rightExpression->getType(context)->getLLVMType(context);

  if (!leftLLVMType->isIntegerTy()) {
    context.reportError(expression->getLine(), "Left expression in " + expression->getOperation() +
                        " operation must be integer type");
    throw 1;
  }
  if (!rightLLVMType->isIntegerTy()) {
    context.reportError(expression->getLine(), "Right expression in " + expression->getOperation() +
                        " operation must be integer type");
    throw 1;
  }
}

