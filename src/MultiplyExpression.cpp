//
//  MultiplyExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MultiplyExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MultiplyExpression::MultiplyExpression(const IExpression* left,
                                       const IExpression* right,
                                       int line) :
mLeft(left), mRight(right), mLine(line) {}

MultiplyExpression::~MultiplyExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* MultiplyExpression::getLeft() const {
  return mLeft;
}

const IExpression* MultiplyExpression::getRight() const {
  return mRight;
}

string MultiplyExpression::getOperation() const {
  return "*";
}

int MultiplyExpression::getLine() const {
  return mLine;
}

Value* MultiplyExpression::generateIR(IRGenerationContext& context,
                                      const IType* assignToType) const {
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  checkTypes(context, leftType, rightType, '*', mLine);
  
  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  
  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
  leftType == PrimitiveTypes::DOUBLE ||
  rightType == PrimitiveTypes::FLOAT ||
  rightType == PrimitiveTypes::DOUBLE;
  Instruction::BinaryOps instruction = isFloat ? Instruction::FMul : Instruction::Mul;
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, "mul");
}

const IType* MultiplyExpression::getType(IRGenerationContext& context) const {
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  checkTypes(context, leftType, rightType, '*', mLine);
  
  return leftType->canAutoCastTo(context, rightType) ? rightType : leftType;
}

bool MultiplyExpression::isConstant() const {
  return false;
}

bool MultiplyExpression::isAssignable() const {
  return false;
}

void MultiplyExpression::printToStream(IRGenerationContext& context,
                                       std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
