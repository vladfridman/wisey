//
//  WhileStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/WhileStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* WhileStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* whileCond = BasicBlock::Create(context.getLLVMContext(), "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(context.getLLVMContext(), "while.body", function);
  BasicBlock* whileEnd = BasicBlock::Create(context.getLLVMContext(), "while.end", function);
  
  BranchInst::Create(whileCond, context.getBasicBlock());
  context.setBasicBlock(whileCond);
  Value* conditionValue = mConditionExpression.generateIR(context);
  BranchInst::Create(whileBody, whileEnd, conditionValue, context.getBasicBlock());

  context.setBasicBlock(whileBody);
  mStatement.generateIR(context);
  BranchInst::Create(whileCond, context.getBasicBlock());

  context.setBasicBlock(whileEnd);

  return conditionValue;
}
