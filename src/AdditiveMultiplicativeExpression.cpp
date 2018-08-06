//
//  AdditiveMultiplicativeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IPrintStatement.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringFormatType.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AdditiveMultiplicativeExpression::
AdditiveMultiplicativeExpression(const IExpression* leftExpression,
                                 int operation,
                                 const IExpression* rightExpression,
                                 int line) :
mLeftExpression(leftExpression),
mRightExpression(rightExpression),
mOperation(operation),
mLine(line) { }

AdditiveMultiplicativeExpression::~AdditiveMultiplicativeExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

int AdditiveMultiplicativeExpression::getLine() const {
  return mLine;
}

const IExpression* AdditiveMultiplicativeExpression::getLeft() const {
  return mLeftExpression;
}

const IExpression* AdditiveMultiplicativeExpression::getRight() const {
  return mRightExpression;
}

Value* AdditiveMultiplicativeExpression::generateIR(IRGenerationContext& context,
                                                    const IType* assignToType) const {
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  checkTypes(context, leftType, rightType);

  Value* leftValue = mLeftExpression->generateIR(context, PrimitiveTypes::VOID);
  Value* rightValue = mRightExpression->generateIR(context, PrimitiveTypes::VOID);
  
  if (leftType->isPointer()) {
    Value* index[1];
    index[0] = rightValue;
    return IRWriter::createGetElementPtrInst(context, leftValue, index);
  }

  if (mOperation == '+' &&
      (StringType::isStringVariation(context, leftType, mLine) ||
       StringType::isStringVariation(context, rightType, mLine))) {
        assert(false && "attempting to get value of a stringformat type expression");
  }
  Instruction::BinaryOps instruction;
  string name;
  bool isFloat = leftType == PrimitiveTypes::FLOAT ||
    leftType == PrimitiveTypes::DOUBLE ||
    rightType == PrimitiveTypes::FLOAT ||
    rightType == PrimitiveTypes::DOUBLE;
  switch (mOperation) {
    case '+': name = "add"; instruction = isFloat ? Instruction::FAdd : Instruction::Add; break;
    default: return NULL;
  }
  
  if (leftType->canAutoCastTo(context, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType, mLine);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType, mLine);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, name);
}

const IType* AdditiveMultiplicativeExpression::getType(IRGenerationContext& context) const {
  const IType* leftType = mLeftExpression->getType(context);
  const IType* rightType = mRightExpression->getType(context);
  checkTypes(context, leftType, rightType);
  
  if (leftType->isPointer() && rightType == leftType) {
    return PrimitiveTypes::LONG;
  }
  
  if (leftType->isPointer()) {
    return leftType;
  }

  if (mOperation == '+' &&
      (StringType::isStringVariation(context, leftType, mLine) ||
       StringType::isStringVariation(context, rightType, mLine))) {
    return PrimitiveTypes::STRING_FORMAT;
  }
  
  if (leftType->canAutoCastTo(context, rightType)) {
    return rightType;
  }
  return leftType;
}

void AdditiveMultiplicativeExpression::checkTypes(IRGenerationContext& context,
                                                  const IType* leftType,
                                                  const IType* rightType) const {
  if (leftType == PrimitiveTypes::VOID || rightType == PrimitiveTypes::VOID) {
    context.reportError(mLine, "Can not use expressions of type VOID in a '" +
                        string(1, mOperation) + "' operation");
    throw 1;
  }
  
  if (isPointerArithmetic(leftType, rightType)) {
    return;
  }

  if (mOperation == '+' && StringType::isStringVariation(context, leftType, mLine) &&
      rightType->isPrimitive()) {
    return;
  }
  
  if (mOperation == '+' && StringType::isStringVariation(context, rightType, mLine) &&
      leftType->isPrimitive()) {
    return;
  }
 
  if (mOperation == '+' && StringType::isStringVariation(context, leftType, mLine) &&
      StringType::isStringVariation(context, rightType, mLine)) {
    return;
  }

  if (!leftType->isPrimitive()  || !rightType->isPrimitive()) {
    context.reportError(mLine, "Can not do operation '" + string(1, mOperation) +
                        "' on non-primitive types");
    throw 1;
  }
  
  if (!leftType->canCastTo(context, rightType) && !rightType->canCastTo(context, leftType)) {
    context.reportError(mLine, "Incompatible types in '" + string(1, mOperation) + "' operation");
    throw 1;
  }
  
  if (!leftType->canAutoCastTo(context, rightType) && !rightType->canAutoCastTo(context, leftType)) {
    context.reportError(mLine, "Incompatible types in '" + string(1, mOperation) +
                        "' operation that require an explicit cast");
    throw 1;
  }
}

bool AdditiveMultiplicativeExpression::isConstant() const {
  return false;
}

bool AdditiveMultiplicativeExpression::isAssignable() const {
  return false;
}

void AdditiveMultiplicativeExpression::printToStream(IRGenerationContext& context,
                                                     std::iostream& stream) const {
  mLeftExpression->printToStream(context, stream);
  stream << " " << (char) mOperation << " ";
  mRightExpression->printToStream(context, stream);
}

bool AdditiveMultiplicativeExpression::isPointerArithmetic(const IType* leftType,
                                                           const IType* rightType) const {
  if (leftType->isPointer() &&
      (rightType == PrimitiveTypes::INT || rightType == PrimitiveTypes::LONG)) {
    return mOperation == '+' || mOperation == '-' || mOperation == '%';
  }
  if (leftType->isPointer() && rightType == leftType && mOperation == '-') {
    return true;
  }
  
  return false;
}
