//
//  BitwiseXorExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "BitwiseXorExpression.hpp"
#include "IRWriter.hpp"
#include "IntExpressionChecker.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseXorExpression::BitwiseXorExpression(const IExpression* left,
                                           const IExpression* right,
                                           int line) :
mLeft(left), mRight(right), mLine(line) {}

BitwiseXorExpression::~BitwiseXorExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* BitwiseXorExpression::getLeft() const {
  return mLeft;
}

const IExpression* BitwiseXorExpression::getRight() const {
  return mRight;
}

string BitwiseXorExpression::getOperation() const {
  return "^";
}

int BitwiseXorExpression::getLine() const {
  return mLine;
}

Value* BitwiseXorExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IntExpressionChecker::checkTypes(context, this);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, Instruction::Xor, leftValue, rightValue, "");
}

const IType* BitwiseXorExpression::getType(IRGenerationContext& context) const {
  IntExpressionChecker::checkTypes(context, this);

  return mLeft->getType(context);
}

bool BitwiseXorExpression::isConstant() const {
  return false;
}

bool BitwiseXorExpression::isAssignable() const {
  return false;
}

void BitwiseXorExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
