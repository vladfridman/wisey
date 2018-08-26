//
//  BitwiseAndExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/BitwiseAndExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntExpressionChecker.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseAndExpression::BitwiseAndExpression(const IExpression* left,
                                           const IExpression* right,
                                           int line) :
mLeft(left), mRight(right), mLine(line) {}

BitwiseAndExpression::~BitwiseAndExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* BitwiseAndExpression::getLeft() const {
  return mLeft;
}

const IExpression* BitwiseAndExpression::getRight() const {
  return mRight;
}

string BitwiseAndExpression::getOperation() const {
  return "&";
}

int BitwiseAndExpression::getLine() const {
  return mLine;
}

Value* BitwiseAndExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IntExpressionChecker::checkTypes(context, this);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, Instruction::And, leftValue, rightValue, "");
}

const IType* BitwiseAndExpression::getType(IRGenerationContext& context) const {
  IntExpressionChecker::checkTypes(context, this);
  
  return mLeft->getType(context);
}

bool BitwiseAndExpression::isConstant() const {
  return false;
}

bool BitwiseAndExpression::isAssignable() const {
  return false;
}

void BitwiseAndExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
