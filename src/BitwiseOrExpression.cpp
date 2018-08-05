//
//  BitwiseOrExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/BitwiseOrExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BitwiseOrExpression::BitwiseOrExpression(const IExpression* left,
                                         const IExpression* right,
                                         int line) :
mLeft(left), mRight(right), mLine(line) {}

BitwiseOrExpression::~BitwiseOrExpression() {
}

int BitwiseOrExpression::getLine() const {
  return mLine;
}

Value* BitwiseOrExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  checkTypes(context);
  
  Value* leftValue = mLeft->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRight->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeft->getType(context);
  const IType* rightType = mRight->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, Instruction::Or, leftValue, rightValue, "");
}

void BitwiseOrExpression::checkTypes(IRGenerationContext& context) const {
  Type* leftLLVMType = mLeft->getType(context)->getLLVMType(context);
  Type* rightLLVMType = mRight->getType(context)->getLLVMType(context);
  if (!leftLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Left expression in bitwise OR operation must be integer type");
    throw 1;
  }
  if (!rightLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Right expression in bitwise OR operation must be integer type");
    throw 1;
  }
}

const IType* BitwiseOrExpression::getType(IRGenerationContext& context) const {
  checkTypes(context);
  
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
  mLeft->printToStream(context, stream);
  stream << " | ";
  mRight->printToStream(context, stream);
}
