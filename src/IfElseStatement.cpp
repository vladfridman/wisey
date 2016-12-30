//
//  IfElseStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IfElseStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* IfElseStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifElse = BasicBlock::Create(context.getLLVMContext(), "if.else", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);

  Value* conditionValue = mCondition.generateIR(context);
  BranchInst::Create(ifThen, ifElse, conditionValue, context.getBasicBlock());
  
  context.setBasicBlock(ifThen);
  mThenStatement.generateIR(context);
  BranchInst::Create(ifEnd, context.getBasicBlock());
  
  context.setBasicBlock(ifElse);
  mElseStatement.generateIR(context);
  BranchInst::Create(ifEnd, context.getBasicBlock());
  
  context.setBasicBlock(ifEnd);

  return conditionValue;
}

