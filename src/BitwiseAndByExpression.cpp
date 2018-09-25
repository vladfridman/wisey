//
//  BitwiseAndByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/26/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "BitwiseAndByExpression.hpp"
#include "FakeExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntExpressionChecker.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseAndByExpression::BitwiseAndByExpression(const IExpression* expression,
                                               const IExpression* adjustment,
                                               int line) :
mExpression(expression),
mAdjustment(adjustment),
mLine(line) { }

BitwiseAndByExpression::~BitwiseAndByExpression() {
  delete mExpression;
  delete mAdjustment;
}

const IExpression* BitwiseAndByExpression::getLeft() const {
  return mExpression;
}

const IExpression* BitwiseAndByExpression::getRight() const {
  return mAdjustment;
}

string BitwiseAndByExpression::getOperation() const {
  return "&=";
}

int BitwiseAndByExpression::getLine() const {
  return mLine;
}

Value* BitwiseAndByExpression::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  IExpression::checkForUndefined(context, mExpression);

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
  IRWriter::createBinaryOperator(context, Instruction::And, originalValue, adjustment, "");
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((const IExpressionAssignable*) mExpression)->
  getVariable(context, arrayIndices);
  
  FakeExpression fakeExpression(newValue, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);
  
  return newValue;
}

const IType* BitwiseAndByExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool BitwiseAndByExpression::isConstant() const {
  return false;
}

bool BitwiseAndByExpression::isAssignable() const {
  return false;
}

void BitwiseAndByExpression::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}

