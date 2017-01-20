//
//  WhileStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/SafeBranch.hpp"
#include "yazyk/WhileStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* WhileStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* whileCond = BasicBlock::Create(context.getLLVMContext(), "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(context.getLLVMContext(), "while.body", function);
  BasicBlock* whileEnd = BasicBlock::Create(context.getLLVMContext(), "while.end", function);
  
  SafeBranch::newBranch(whileCond, context);
  context.setBasicBlock(whileCond);
  Value* conditionValue = mConditionExpression.generateIR(context);
  SafeBranch::newConditionalBranch(whileBody, whileEnd, conditionValue, context);
  
  context.setBasicBlock(whileBody);
  scopes.setBreakToBlock(whileEnd);
  scopes.setContinueToBlock(whileCond);
  mStatement.generateIR(context);
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);

  SafeBranch::newBranch(whileCond, context);

  context.setBasicBlock(whileEnd);

  return conditionValue;
}
