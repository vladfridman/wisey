//
//  AddditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Value.h>

#include "yazyk/AddditiveMultiplicativeExpression.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* AddditiveMultiplicativeExpression::generateIR(IRGenerationContext& context) {
  Instruction::BinaryOps instruction;
  string name;
  switch (operation) {
    case '+': name = "add"; instruction = Instruction::Add; break;
    case '-': name = "sub"; instruction = Instruction::Sub; break;
    case '*': name = "mul"; instruction = Instruction::Mul; break;
    case '/': name = "div"; instruction = Instruction::SDiv; break;
    default: return NULL;
  }
  
  Value * lhsValue = lhs.generateIR(context);
  Value * rhsValue = rhs.generateIR(context);
  
  BasicBlock * currentBlock = context.currentBlock();
  
  return mLLVMBridge->createBinaryOperator(instruction,
                                           lhsValue,
                                           rhsValue,
                                           name,
                                           currentBlock);
}
