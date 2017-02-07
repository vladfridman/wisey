//
//  AdditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "yazyk/AdditiveMultiplicativeExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* AdditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) const {
  checkTypes(context);

  Instruction::BinaryOps instruction;
  string name;
  switch (mOperation) {
    case '+': name = "add"; instruction = Instruction::Add; break;
    case '-': name = "sub"; instruction = Instruction::Sub; break;
    case '*': name = "mul"; instruction = Instruction::Mul; break;
    case '/': name = "div"; instruction = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value * leftValue = mLeftExpression.generateIR(context);
  Value * rightValue = mRightExpression.generateIR(context);
  
  return llvm::BinaryOperator::Create(instruction,
                                      leftValue,
                                      rightValue,
                                      name,
                                      context.getBasicBlock());
}

IType* AdditiveMultiplicativeExpression::getType(IRGenerationContext& context) const {
  return mLeftExpression.getType(context);
}

// TODO: implement a more sensible type checking/casting
void AdditiveMultiplicativeExpression::checkTypes(IRGenerationContext& context) const {
  IType* leftExpressionType = mLeftExpression.getType(context);
  IType* rightExpressionType = mRightExpression.getType(context);
  
  if (leftExpressionType != rightExpressionType) {
    Log::e("Incopatible types in '" + string(1, mOperation) + "' operation");
    exit(1);
  }
  
  if (leftExpressionType->getTypeKind() != PRIMITIVE_TYPE) {
    Log::e("Can not do operation '" + string(1, mOperation) + "' on non-primitive types");
    exit(1);
  }
  
  if (leftExpressionType == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not use expressions of type VOID in a '" + string(1, mOperation) + "' operation");
    exit(1);
  }
}
