//
//  ShiftRightExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
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
  checkTypes(context);
  
  Value* leftValue = mLeftExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRightExpression->generateIR(context, PrimitiveTypes::VOID);
  
  return IRWriter::createBinaryOperator(context, Instruction::AShr, leftValue, rightValue, "");
}

const IType* ShiftRightExpression::getType(IRGenerationContext& context) const {
  checkTypes(context);
  
  return mLeftExpression->getType(context);
}

void ShiftRightExpression::checkTypes(IRGenerationContext& context) const {
  Type* leftLLVMType = mLeftExpression->getType(context)->getLLVMType(context);
  Type* rightLLVMType = mRightExpression->getType(context)->getLLVMType(context);
  if (!leftLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Left expression in shift right operation must be integer type");
    throw 1;
  }
  if (!rightLLVMType->isIntegerTy()) {
    context.reportError(mLine, "Right expression in shift right operation must be integer type");
    throw 1;
  }
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
