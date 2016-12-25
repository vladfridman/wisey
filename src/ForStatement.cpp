//
//  ForStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ForStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* ForStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* forCond = BasicBlock::Create(context.getLLVMContext(), "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(context.getLLVMContext(), "for.body", function);
  BasicBlock* forInc = BasicBlock::Create(context.getLLVMContext(), "for.inc", function);
  BasicBlock* forEnd = BasicBlock::Create(context.getLLVMContext(), "for.end", function);
  
  mStartStatement.generateIR(context);
  BranchInst::Create(forCond, context.currentBlock());
  
  context.replaceBlock(forCond);
  Value* conditionValue = mConditionStatement.generateIR(context);
  BranchInst::Create(forBody, forEnd, conditionValue, context.currentBlock());
  
  context.replaceBlock(forBody);
  mBodyStatement.generateIR(context);
  BranchInst::Create(forInc, context.currentBlock());
  
  context.replaceBlock(forInc);
  mIncrementExpression.generateIR(context);
  BranchInst::Create(forCond, context.currentBlock());

  context.replaceBlock(forEnd);
  
  return conditionValue;
}
