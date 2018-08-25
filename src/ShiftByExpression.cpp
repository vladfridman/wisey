//
//  ShiftByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntExpressionChecker.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ShiftByExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ShiftByExpression::ShiftByExpression(const IExpression* expression,
                                     const IExpression* adjustment,
                                     bool isLeftShift,
                                     int line) :
mExpression(expression),
mAdjustment(adjustment),
mIsLeftShift(isLeftShift),
mLine(line) { }

ShiftByExpression::~ShiftByExpression() {
  delete mExpression;
  delete mAdjustment;
}

ShiftByExpression* ShiftByExpression::newShiftLeftBy(const IExpression* expression,
                                                     const IExpression* adjustment,
                                                     int line) {
  return new ShiftByExpression(expression, adjustment, true, line);
}

ShiftByExpression* ShiftByExpression::newShiftRightBy(const IExpression* expression,
                                                      const IExpression* adjustment,
                                                      int line) {
  return new ShiftByExpression(expression, adjustment, false, line);
}

const IExpression* ShiftByExpression::getLeft() const {
  return mExpression;
}

const IExpression* ShiftByExpression::getRight() const {
  return mAdjustment;
}

string ShiftByExpression::getOperation() const {
  return mIsLeftShift ? "<<=" : ">>=";
}

int ShiftByExpression::getLine() const {
  return mLine;
}

Value* ShiftByExpression::generateIR(IRGenerationContext& context,
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

  Instruction::BinaryOps instruction = mIsLeftShift ? Instruction::Shl : Instruction::AShr;
  Value* shiftByResult =
  IRWriter::createBinaryOperator(context, instruction, originalValue, adjustment, "");
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((const IExpressionAssignable*) mExpression)->
  getVariable(context, arrayIndices);
  
  FakeExpression fakeExpression(shiftByResult, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);
  
  return shiftByResult;
}

const IType* ShiftByExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool ShiftByExpression::isConstant() const {
  return false;
}

bool ShiftByExpression::isAssignable() const {
  return false;
}

void ShiftByExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
