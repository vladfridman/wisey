//
//  IntExpressionChecker.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IntExpressionChecker.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IntExpressionChecker::checkTypes(IRGenerationContext& context,
                                      const IBinaryExpression* expression) {
  Type* leftLLVMType = expression->getLeft()->getType(context)->getLLVMType(context);
  Type* rightLLVMType = expression->getRight()->getType(context)->getLLVMType(context);
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

