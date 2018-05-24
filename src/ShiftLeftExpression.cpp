//
//  ShiftLeftExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ShiftLeftExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ShiftLeftExpression::ShiftLeftExpression(const IExpression* leftExpression,
                                         const IExpression* rightExpression,
                                         int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mLine(line) { }

ShiftLeftExpression::~ShiftLeftExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int ShiftLeftExpression::getLine() const {
  return mLine;
}

Value* ShiftLeftExpression::generateIR(IRGenerationContext& context,
                                                    const IType* assignToType) const {
  checkTypes(context);
  
  Value* leftValue = mLeftExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRightExpression->generateIR(context, PrimitiveTypes::VOID);
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  if (rightType != leftType) {
    rightValue = rightType->castTo(context, rightValue, leftType, mLine);
  }

  return IRWriter::createBinaryOperator(context, Instruction::Shl, leftValue, rightValue, "");
}

const IType* ShiftLeftExpression::getType(IRGenerationContext& context) const {
  checkTypes(context);

  return mLeftExpression->getType(context);
}

void ShiftLeftExpression::checkTypes(IRGenerationContext& context) const {
  Type* leftLLVMType = mLeftExpression->getType(context)->getLLVMType(context);
  Type* rightLLVMType = mRightExpression->getType(context)->getLLVMType(context);
  if (!leftLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Left expression in shift left operation must be integer type");
    throw 1;
  }
  if (!rightLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Right expression in shift left operation must be integer type");
    throw 1;
  }
}

bool ShiftLeftExpression::isConstant() const {
  return false;
}

bool ShiftLeftExpression::isAssignable() const {
  return false;
}

void ShiftLeftExpression::printToStream(IRGenerationContext& context,
                                                     std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " << ";
  mRightExpression->printToStream(context, stream);
}
