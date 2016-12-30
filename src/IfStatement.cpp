//
//  IfStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IfStatement.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value* IfStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);
  
  Value* conditionValue = mCondition.generateIR(context);
  SafeBranch::newConditionalBranch(ifThen, ifEnd, conditionValue, context);
  
  context.setBasicBlock(ifThen);
  mThenStatement.generateIR(context);
  SafeBranch::newBranch(ifEnd, context);
  
  context.setBasicBlock(ifEnd);
  
  return conditionValue;
}

