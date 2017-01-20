//
//  DoWhileStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/DoStatement.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value* DoStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* doCond = BasicBlock::Create(context.getLLVMContext(), "do.cond", function);
  BasicBlock* doBody = BasicBlock::Create(context.getLLVMContext(), "do.body", function);
  BasicBlock* doEnd = BasicBlock::Create(context.getLLVMContext(), "do.end", function);
  
  SafeBranch::newBranch(doBody, context);
  
  
  context.setBasicBlock(doBody);
  scopes.setBreakToBlock(doEnd);
  scopes.setContinueToBlock(doBody);
  mStatement.generateIR(context);
  SafeBranch::newBranch(doCond, context);
  
  context.setBasicBlock(doCond);
  Value* conditionValue = mConditionExpression.generateIR(context);
  SafeBranch::newConditionalBranch(doBody, doEnd, conditionValue, context);
  
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);
  
  context.setBasicBlock(doEnd);
  
  return conditionValue;
}
