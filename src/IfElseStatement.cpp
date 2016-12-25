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
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifElse = BasicBlock::Create(context.getLLVMContext(), "if.else", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);

  Value* conditionValue = mCondition.generateIR(context);
  BranchInst::Create(ifThen, ifElse, conditionValue, context.currentBlock());
  
  context.replaceBlock(ifThen);
  mThenStatement.generateIR(context);
  BranchInst::Create(ifEnd, context.currentBlock());
  
  context.replaceBlock(ifElse);
  mElseStatement.generateIR(context);
  BranchInst::Create(ifEnd, context.currentBlock());
  
  context.replaceBlock(ifEnd);

  return conditionValue;
}

