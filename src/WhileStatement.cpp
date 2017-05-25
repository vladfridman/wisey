//
//  WhileStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRWriter.hpp"
#include "wisey/WhileStatement.hpp"

using namespace llvm;
using namespace wisey;

void WhileStatement::prototypeObjects(IRGenerationContext& context) const {
}

Value* WhileStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* whileCond = BasicBlock::Create(context.getLLVMContext(), "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(context.getLLVMContext(), "while.body", function);
  BasicBlock* whileEnd = BasicBlock::Create(context.getLLVMContext(), "while.end", function);
  
  IRWriter::createBranch(context, whileCond);
  context.setBasicBlock(whileCond);
  Value* conditionValue = mConditionExpression.generateIR(context);
  IRWriter::createConditionalBranch(context, whileBody, whileEnd, conditionValue);
  
  context.setBasicBlock(whileBody);
  scopes.setBreakToBlock(whileEnd);
  scopes.setContinueToBlock(whileCond);
  mStatement.generateIR(context);
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);

  IRWriter::createBranch(context, whileCond);

  context.setBasicBlock(whileEnd);

  return conditionValue;
}
