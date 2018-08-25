//
//  ShiftRightExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntExpressionChecker.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ShiftRightExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ShiftRightExpression::ShiftRightExpression(const IExpression* leftExpression,
                                         const IExpression* rightExpression,
                                         int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mLine(line) { }

ShiftRightExpression::~ShiftRightExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int ShiftRightExpression::getLine() const {
  return mLine;
}

Value* ShiftRightExpression::generateIR(IRGenerationContext& context,
                                       const IType* assignToType) const {
  IntExpressionChecker::checkTypes(context, this);
  
  Value* leftValue = mLeftExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRightExpression->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }

  return IRWriter::createBinaryOperator(context, Instruction::AShr, leftValue, rightValue, "");
}

const IType* ShiftRightExpression::getType(IRGenerationContext& context) const {
  IntExpressionChecker::checkTypes(context, this);

  return mLeftExpression->getType(context);
}

bool ShiftRightExpression::isConstant() const {
  return false;
}

bool ShiftRightExpression::isAssignable() const {
  return false;
}

void ShiftRightExpression::printToStream(IRGenerationContext& context,
                                        std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " >> ";
  mRightExpression->printToStream(context, stream);
}

const IExpression* ShiftRightExpression::getLeft() const {
  return mLeftExpression;
}

const IExpression* ShiftRightExpression::getRight() const {
  return mRightExpression;
}

string ShiftRightExpression::getOperation() const {
  return ">>";
}
