//
//  SubtractExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "AutoCast.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"
#include "SubtractExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

SubtractExpression::SubtractExpression(const IExpression* left,
                                       const IExpression* right,
                                       int line) :
mLeft(left), mRight(right), mLine(line) {}

SubtractExpression::~SubtractExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* SubtractExpression::getLeft() const {
  return mLeft;
}

const IExpression* SubtractExpression::getRight() const {
  return mRight;
}

string SubtractExpression::getOperation() const {
  return "-";
}

int SubtractExpression::getLine() const {
  return mLine;
}

Value* SubtractExpression::generateIR(IRGenerationContext& context,
                                      const IType* assignToType) const {
  checkTypes(context, mLeft, mRight, '-', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);

  if (leftType->isPointer() && rightType == leftType) {
    return subtractTwoPointers(context, leftType, leftValue, rightType, rightValue);
  }

  if (leftType->isPointer()) {
    return subtractIntFromPointer(context, leftType, leftValue, rightType, rightValue);
  }

  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
  leftType == PrimitiveTypes::DOUBLE ||
  rightType == PrimitiveTypes::FLOAT ||
  rightType == PrimitiveTypes::DOUBLE;
  Instruction::BinaryOps instruction = isFloat ? Instruction::FSub : Instruction::Sub;
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, "sub");
}

const IType* SubtractExpression::getType(IRGenerationContext& context) const {
  checkTypes(context, mLeft, mRight, '-', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  if (leftType->isPointer() && rightType == leftType) {
    return PrimitiveTypes::LONG;
  }
  
  if (leftType->isPointer()) {
    return leftType;
  }

  return leftType->canAutoCastTo(context, rightType) ? rightType : leftType;
}

bool SubtractExpression::isConstant() const {
  return false;
}

bool SubtractExpression::isAssignable() const {
  return false;
}

void SubtractExpression::printToStream(IRGenerationContext& context,
                                       std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}

Value* SubtractExpression::subtractTwoPointers(IRGenerationContext& context,
                                               const IType* leftType,
                                               Value* leftValue,
                                               const IType* rightType,
                                               Value* rightValue) const {
  Value* leftLongValue = leftType->castTo(context, leftValue, PrimitiveTypes::LONG, mLine);
  Value* rightLongValue = leftType->castTo(context, rightValue, PrimitiveTypes::LONG, mLine);
  return IRWriter::createBinaryOperator(context,
                                        Instruction::Sub,
                                        leftLongValue,
                                        rightLongValue,
                                        "sub");
}

Value* SubtractExpression::subtractIntFromPointer(IRGenerationContext& context,
                                                  const IType* leftType,
                                                  Value* leftValue,
                                                  const IType* rightType,
                                                  Value* rightValue) const {
  Value* index[1];
  Value* zero = ConstantInt::get(rightType->getLLVMType(context), 0);
  Value* negated = IRWriter::createBinaryOperator(context,
                                                  Instruction::Sub,
                                                  zero,
                                                  rightValue,
                                                  "sub");
  index[0] = negated;
  return IRWriter::createGetElementPtrInst(context, leftValue, index);
}
