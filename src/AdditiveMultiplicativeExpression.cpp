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
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

AdditiveMultiplicativeExpression::~AdditiveMultiplicativeExpression() {
  delete mLeftExpression;
  delete mRightExpression;
}

Value* AdditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) const {
  IType* leftType = mLeftExpression->getType(context);
  IType* rightType = mRightExpression->getType(context);
  checkTypes(leftType, rightType);

  Instruction::BinaryOps instruction;
  string name;
  bool isFloat = leftType == PrimitiveTypes::FLOAT_TYPE ||
    leftType == PrimitiveTypes::DOUBLE_TYPE ||
    rightType == PrimitiveTypes::FLOAT_TYPE ||
    rightType == PrimitiveTypes::DOUBLE_TYPE;
  switch (mOperation) {
    case '+': name = "add"; instruction = isFloat ? Instruction::FAdd : Instruction::Add; break;
    case '-': name = "sub"; instruction = isFloat ? Instruction::FSub : Instruction::Sub; break;
    case '*': name = "mul"; instruction = isFloat ? Instruction::FMul : Instruction::Mul; break;
    case '/': name = "div"; instruction = isFloat ? Instruction::FDiv : Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value* leftValue = mLeftExpression->generateIR(context);
  Value* rightValue = mRightExpression->generateIR(context);
  
  if (leftType->canAutoCastTo(rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType);
  }
  
  return IRWriter::createBinaryOperator(context, instruction, leftValue, rightValue, name);
}

IType* AdditiveMultiplicativeExpression::getType(IRGenerationContext& context) const {
  IType* leftType = mLeftExpression->getType(context);
  IType* rightType = mRightExpression->getType(context);
  checkTypes(leftType, rightType);

  if (leftType->canAutoCastTo(rightType)) {
    return rightType;
  }
  return leftType;
}

void AdditiveMultiplicativeExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an additive/multiplicative expression, "
         "it is not a heap pointer");
  exit(1);
}

// TODO: implement a more sensible type checking/casting
void AdditiveMultiplicativeExpression::checkTypes(IType* leftType, IType* rightType) const {
  if (leftType == PrimitiveTypes::VOID_TYPE || rightType == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not use expressions of type VOID in a '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (leftType->getTypeKind() != PRIMITIVE_TYPE || rightType->getTypeKind() != PRIMITIVE_TYPE) {
    Log::e("Can not do operation '" + string(1, mOperation) + "' on non-primitive types");
    exit(1);
  }

  if (!leftType->canCastTo(rightType) && !rightType->canCastTo(leftType)) {
    Log::e("Incopatible types in '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (!leftType->canAutoCastTo(rightType) && !rightType->canAutoCastTo(leftType)) {
    Log::e("Incopatible types in '" + string(1, mOperation) +
           "' operation that require an explicit cast");
    exit(1);
  }
}
