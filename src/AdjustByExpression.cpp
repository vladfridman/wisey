//
//  AdjustByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "AdjustByExpression.hpp"
#include "AutoCast.hpp"
#include "FakeExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AdjustByExpression::AdjustByExpression(const IExpression* expression,
                                       const IExpression* adjustment,
                                       string variableName,
                                       bool isIncrement,
                                       int line) :
mExpression(expression),
mAdjustment(adjustment),
mVariableName(variableName),
mIsIncrement(isIncrement),
mLine(line) { }

AdjustByExpression::~AdjustByExpression() {
  delete mExpression;
  delete mAdjustment;
}

const IExpression* AdjustByExpression::getLeft() const {
  return mExpression;
}

const IExpression* AdjustByExpression::getRight() const {
  return mAdjustment;
}

string AdjustByExpression::getOperation() const {
  return mIsIncrement ? "+=" : "-=";
}

int AdjustByExpression::getLine() const {
  return mLine;
}

AdjustByExpression* AdjustByExpression::newIncrementBy(const IExpression* expression,
                                                       const IExpression* adjustment,
                                                       int line) {
  return new AdjustByExpression(expression, adjustment, "inc", true, line);
}

AdjustByExpression* AdjustByExpression::newDecrementBy(const IExpression* expression,
                                                       const IExpression* adjustment,
                                                       int line) {
  return new AdjustByExpression(expression, adjustment, "dec", false, line);
}

Value* AdjustByExpression::generateIR(IRGenerationContext& context,
                                      const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  IExpression::checkForUndefined(context, mExpression);
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, "Increment/decrement operation may only be applied to variables");
    throw 1;
  }
  if (!PrimitiveTypes::isNumberType(expressionType)) {
    context.reportError(mLine, getOperation() + " operation may only be applied to number types");
    throw 1;
  }
  
  Value* originalValue = mExpression->generateIR(context, assignToType);
  Value* adjustment = mAdjustment->generateIR(context, expressionType);
  const IType* adjustmentType = mAdjustment->getType(context);
  Value* adjustmentCast =
  AutoCast::maybeCast(context, adjustmentType, adjustment, expressionType, mLine);

  Instruction::BinaryOps instruction;
  
  if (mIsIncrement) {
    instruction = PrimitiveTypes::isFloatType(expressionType)
    ? Instruction::FAdd : Instruction::Add;
  } else {
    instruction = PrimitiveTypes::isFloatType(expressionType)
    ? Instruction::FSub : Instruction::Sub;
  }

  Value* incrementResult =
  IRWriter::createBinaryOperator(context,
                                 instruction,
                                 originalValue,
                                 adjustmentCast,
                                 mVariableName);

  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((const IExpressionAssignable*) mExpression)->
  getVariable(context, arrayIndices);
  
  FakeExpression fakeExpression(incrementResult, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);
  
  return incrementResult;
}

const IType* AdjustByExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool AdjustByExpression::isConstant() const {
  return false;
}

bool AdjustByExpression::isAssignable() const {
  return false;
}

void AdjustByExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
