//
//  AddditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "yazyk/AddditiveMultiplicativeExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* AddditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) const {
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
