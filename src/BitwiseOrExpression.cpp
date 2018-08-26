//
//  BitwiseOrExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/BitwiseOrExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntExpressionChecker.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseOrExpression::BitwiseOrExpression(const IExpression* left,
                                         const IExpression* right,
                                         int line) :
mLeft(left), mRight(right), mLine(line) {}

BitwiseOrExpression::~BitwiseOrExpression() {
  delete mLeft;
  delete mRight;
}

const IExpression* BitwiseOrExpression::getLeft() const {
  return mLeft;
}

const IExpression* BitwiseOrExpression::getRight() const {
  return mRight;
}

string BitwiseOrExpression::getOperation() const {
  return "|";
}

int BitwiseOrExpression::getLine() const {
  return mLine;
}

Value* BitwiseOrExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  IntExpressionChecker::checkTypes(context, this);

  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, Instruction::Or, leftValue, rightValue, "");
}

const IType* BitwiseOrExpression::getType(IRGenerationContext& context) const {
  IntExpressionChecker::checkTypes(context, this);

  return mLeft->getType(context);
}

bool BitwiseOrExpression::isConstant() const {
  return false;
}

bool BitwiseOrExpression::isAssignable() const {
  return false;
}

void BitwiseOrExpression::printToStream(IRGenerationContext& context,
                                        std::iostream& stream) const {
  IBinaryExpression::printToStream(context, stream, this);
}
