//
//  ForStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ForStatement.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value* ForStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* forCond = BasicBlock::Create(context.getLLVMContext(), "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(context.getLLVMContext(), "for.body", function);
  BasicBlock* forInc = BasicBlock::Create(context.getLLVMContext(), "for.inc", function);
  BasicBlock* forEnd = BasicBlock::Create(context.getLLVMContext(), "for.end", function);
  
  mStartStatement.generateIR(context);
  SafeBranch::newBranch(forCond, context);
  
  context.setBasicBlock(forCond);
  Value* conditionValue = mConditionStatement.generateIR(context);
  SafeBranch::newConditionalBranch(forBody, forEnd, conditionValue, context);
  
  context.setBreakToBlock(forEnd);
  context.setBasicBlock(forBody);
  mBodyStatement.generateIR(context);
  context.setBreakToBlock(NULL);

  SafeBranch::newBranch(forInc, context);
  context.setBasicBlock(forInc);
  mIncrementExpression.generateIR(context);

  SafeBranch::newBranch(forCond, context);
  context.setBasicBlock(forEnd);
  
  return conditionValue;
}
