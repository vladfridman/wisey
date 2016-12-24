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

Value* WhileStatement::generateIR(IRGenerationContext& context) {
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* whileCond = BasicBlock::Create(context.getLLVMContext(), "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(context.getLLVMContext(), "while.body", function);
  BasicBlock* whileEnd = BasicBlock::Create(context.getLLVMContext(), "while.end", function);
  
  BranchInst::Create(whileCond, context.currentBlock());
  context.replaceBlock(whileCond);
  Value* conditionValue = mConditionExpression.generateIR(context);
  BranchInst::Create(whileBody, whileEnd, conditionValue, context.currentBlock());

  context.replaceBlock(whileBody);
  mStatement.generateIR(context);
  BranchInst::Create(whileCond, context.currentBlock());

  context.replaceBlock(whileEnd);

  return conditionValue;
}
