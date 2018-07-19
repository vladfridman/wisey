//
//  AdjustByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AdjustByExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AdjustByExpression::AdjustByExpression(IExpression* expression,
                                       IExpression* adjustment,
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

int AdjustByExpression::getLine() const {
  return mLine;
}

AdjustByExpression* AdjustByExpression::newIncrementBy(IExpression* expression,
                                                       IExpression* adjustment,
                                                       int line) {
  return new AdjustByExpression(expression, adjustment, "inc", true, line);
}

AdjustByExpression* AdjustByExpression::newDecrementBy(IExpression* expression,
                                                       IExpression* adjustment,
                                                       int line) {
  return new AdjustByExpression(expression, adjustment, "dec", false, line);
}

IVariable* AdjustByExpression::getVariable(IRGenerationContext& context,
                                           vector<const IExpression*>& arrayIndices) const {
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, "Expression left of the increment/decrement is not assignable");
    throw 1;
  }
  return ((IExpressionAssignable*) mExpression)->getVariable(context, arrayIndices);
}

Value* AdjustByExpression::generateIR(IRGenerationContext& context,
                                      const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, "Increment/decrement operation may only be applied to variables");
    throw 1;
  }
  if (expressionType != PrimitiveTypes::INT &&
      expressionType != PrimitiveTypes::CHAR &&
      expressionType != PrimitiveTypes::LONG) {
    context.reportError(mLine,
                        "Expression is of a type that is incompatible with "
                        "increment/decrement operation");
    throw 1;
  }
  
  Value* originalValue = mExpression->generateIR(context, assignToType);
  Value* adjustment = mAdjustment->generateIR(context, expressionType);
  const IType* adjustmentType = mAdjustment->getType(context);
  Value* adjustmentCast =
  AutoCast::maybeCast(context, adjustmentType, adjustment, expressionType, mLine);

  Value* incrementResult =
  IRWriter::createBinaryOperator(context,
                                 mIsIncrement ? Instruction::Add : Instruction::Sub,
                                 originalValue,
                                 adjustmentCast,
                                 mVariableName);

  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((IExpressionAssignable*) mExpression)->getVariable(context, arrayIndices);
  
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
  mExpression->printToStream(context, stream);
  if (mIsIncrement) {
    stream << " += ";
  } else {
    stream << " -= ";
  }
  mAdjustment->printToStream(context, stream);
}
