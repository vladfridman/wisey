//
//  BitwiseOrByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/26/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/BitwiseOrByExpression.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntExpressionChecker.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseOrByExpression::BitwiseOrByExpression(const IExpression* expression,
                                             const IExpression* adjustment,
                                             int line) :
mExpression(expression),
mAdjustment(adjustment),
mLine(line) { }

BitwiseOrByExpression::~BitwiseOrByExpression() {
  delete mExpression;
  delete mAdjustment;
}

const IExpression* BitwiseOrByExpression::getLeft() const {
  return mExpression;
}

const IExpression* BitwiseOrByExpression::getRight() const {
  return mAdjustment;
}

string BitwiseOrByExpression::getOperation() const {
  return "|=";
}

int BitwiseOrByExpression::getLine() const {
  return mLine;
}

Value* BitwiseOrByExpression::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, getOperation() + " operation may only be applied to variables");
    throw 1;
  }
  IntExpressionChecker::checkTypes(context, this);
  
  Value* originalValue = mExpression->generateIR(context, assignToType);
  Value* adjustment = mAdjustment->generateIR(context, expressionType);
  const IType* adjustmentType = mAdjustment->getType(context);
  if (adjustmentType != expressionType) {
    adjustment = adjustmentType->castTo(context, adjustment, expressionType, mLine);
  }
  
  Value* newValue =
  IRWriter::createBinaryOperator(context, Instruction::Or, originalValue, adjustment, "");
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((const IExpressionAssignable*) mExpression)->
  getVariable(context, arrayIndices);
  
  FakeExpression fakeExpression(newValue, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);
  
  return newValue;
}

const IType* BitwiseOrByExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool BitwiseOrByExpression::isConstant() const {
  return false;
}

bool BitwiseOrByExpression::isAssignable() const {
  return false;
}

void BitwiseOrByExpression::printToStream(IRGenerationContext& context,
                                          std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
