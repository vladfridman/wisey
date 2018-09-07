//
//  DivideExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/DivideExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

DivideExpression::DivideExpression(const IExpression* left,
                                   const IExpression* right,
                                   int line) :
mLeft(left), mRight(right), mLine(line) {}

DivideExpression::~DivideExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* DivideExpression::getLeft() const {
  return mLeft;
}

const IExpression* DivideExpression::getRight() const {
  return mRight;
}

string DivideExpression::getOperation() const {
  return "/";
}

int DivideExpression::getLine() const {
  return mLine;
}

Value* DivideExpression::generateIR(IRGenerationContext& context,
                                    const IType* assignToType) const {
  checkTypes(context, mLeft, mRight, '/', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  
  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
  leftType == PrimitiveTypes::DOUBLE ||
  rightType == PrimitiveTypes::FLOAT ||
  rightType == PrimitiveTypes::DOUBLE;
  Instruction::BinaryOps instruction = isFloat ? Instruction::FDiv : Instruction::SDiv;
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, "div");
}

const IType* DivideExpression::getType(IRGenerationContext& context) const {
  checkTypes(context, mLeft, mRight, '/', mLine);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);

  return leftType->canAutoCastTo(context, rightType) ? rightType : leftType;
}

bool DivideExpression::isConstant() const {
  return false;
}

bool DivideExpression::isAssignable() const {
  return false;
}

void DivideExpression::printToStream(IRGenerationContext& context,
                                     std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
