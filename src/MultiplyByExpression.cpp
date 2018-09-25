//
//  MultiplyByExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/26/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "FakeExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntExpressionChecker.hpp"
#include "MultiplyByExpression.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MultiplyByExpression::MultiplyByExpression(const IExpression* expression,
                                           const IExpression* adjustment,
                                           int line) :
mExpression(expression),
mAdjustment(adjustment),
mLine(line) { }

MultiplyByExpression::~MultiplyByExpression() {
  delete mExpression;
  delete mAdjustment;
}

const IExpression* MultiplyByExpression::getLeft() const {
  return mExpression;
}

const IExpression* MultiplyByExpression::getRight() const {
  return mAdjustment;
}

string MultiplyByExpression::getOperation() const {
  return "*=";
}

int MultiplyByExpression::getLine() const {
  return mLine;
}

Value* MultiplyByExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IExpression::checkForUndefined(context, mExpression);

  const IType* expressionType = mExpression->getType(context);
  if (!mExpression->isAssignable()) {
    context.reportError(mLine, getOperation() + " operation may only be applied to variables");
    throw 1;
  }
  if (!PrimitiveTypes::isNumberType(expressionType)) {
    context.reportError(mLine, getOperation() + " operation may only be applied to number types");
    throw 1;
  }
  
  Value* originalValue = mExpression->generateIR(context, assignToType);
  Value* adjustmentValue = mAdjustment->generateIR(context, expressionType);
  const IType* adjustmentType = mAdjustment->getType(context);
  Value* adjustment = AutoCast::maybeCast(context,
                                          adjustmentType,
                                          adjustmentValue,
                                          expressionType,
                                          mLine);
  
  Instruction::BinaryOps instruction = PrimitiveTypes::isFloatType(expressionType)
  ? Instruction::FMul : Instruction::Mul;
  
  Value* newValue =
  IRWriter::createBinaryOperator(context, instruction, originalValue, adjustment, "");
  
  vector<const IExpression*> arrayIndices;
  IVariable* variable = ((const IExpressionAssignable*) mExpression)->
  getVariable(context, arrayIndices);
  
  FakeExpression fakeExpression(newValue, expressionType);
  variable->generateAssignmentIR(context, &fakeExpression, arrayIndices, mLine);
  
  return newValue;
}

const IType* MultiplyByExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool MultiplyByExpression::isConstant() const {
  return false;
}

bool MultiplyByExpression::isAssignable() const {
  return false;
}

void MultiplyByExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
