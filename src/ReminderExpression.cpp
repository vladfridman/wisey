//
//  ReminderExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReminderExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReminderExpression::ReminderExpression(const IExpression* left,
                                       const IExpression* right,
                                       int line) :
mLeft(left), mRight(right), mLine(line) {}

ReminderExpression::~ReminderExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* ReminderExpression::getLeft() const {
  return mLeft;
}

const IExpression* ReminderExpression::getRight() const {
  return mRight;
}

string ReminderExpression::getOperation() const {
  return "%";
}

int ReminderExpression::getLine() const {
  return mLine;
}

Value* ReminderExpression::generateIR(IRGenerationContext& context,
                                      const IType* assignToType) const {
  checkTypes(context, mLeft, mRight, '%', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  
  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
  leftType == PrimitiveTypes::DOUBLE ||
  rightType == PrimitiveTypes::FLOAT ||
  rightType == PrimitiveTypes::DOUBLE;
  Instruction::BinaryOps instruction = isFloat ? Instruction::FRem : Instruction::SRem;

  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, "rem");
}

const IType* ReminderExpression::getType(IRGenerationContext& context) const {
  checkTypes(context, mLeft, mRight, '%', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  if (leftType->isPointer()) {
    return PrimitiveTypes::LONG;
  }

  return leftType->canAutoCastTo(context, rightType) ? rightType : leftType;
}

bool ReminderExpression::isConstant() const {
  return false;
}

bool ReminderExpression::isAssignable() const {
  return false;
}

void ReminderExpression::printToStream(IRGenerationContext& context,
                                       std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
