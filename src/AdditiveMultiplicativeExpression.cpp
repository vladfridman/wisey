//
//  AdditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "yazyk/AdditiveMultiplicativeExpression.hpp"
#include "yazyk/AutoCast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* AdditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) const {
  IType* leftType = mLeftExpression.getType(context);
  IType* rightType = mRightExpression.getType(context);
  checkTypes(leftType, rightType);

  Instruction::BinaryOps instruction;
  string name;
  switch (mOperation) {
    case '+': name = "add"; instruction = Instruction::Add; break;
    case '-': name = "sub"; instruction = Instruction::Sub; break;
    case '*': name = "mul"; instruction = Instruction::Mul; break;
    case '/': name = "div"; instruction = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value* leftValue = mLeftExpression.generateIR(context);
  Value* rightValue = mRightExpression.generateIR(context);
  
  if (AutoCast::canCastLosslessFromTo(leftType, rightType)) {
    leftValue = AutoCast::maybeCast(context, leftType, leftValue, rightType);
  } else {
    rightValue = AutoCast::maybeCast(context, rightType, rightValue, leftType);
  }
  
  return llvm::BinaryOperator::Create(instruction,
                                      leftValue,
                                      rightValue,
                                      name,
                                      context.getBasicBlock());
}

IType* AdditiveMultiplicativeExpression::getType(IRGenerationContext& context) const {
  IType* leftType = mLeftExpression.getType(context);
  IType* rightType = mRightExpression.getType(context);
  checkTypes(leftType, rightType);

  if (AutoCast::canCastLosslessFromTo(leftType, rightType)) {
    return rightType;
  }
  return leftType;
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

  if (!AutoCast::canCast(leftType, rightType) && !AutoCast::canCast(rightType, leftType)) {
    Log::e("Incopatible types in '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (!AutoCast::canCastLosslessFromTo(leftType, rightType) &&
      !AutoCast::canCastLosslessFromTo(rightType, leftType)) {
    Log::e("Incopatible types in '" + string(1, mOperation) +
           "' operation that require an explicit cast");
    exit(1);
  }
}
