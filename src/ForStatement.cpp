//
//  ForStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ForStatement.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace wisey;

void ForStatement::prototypeObjects(IRGenerationContext& context) const {
}

Value* ForStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* forCond = BasicBlock::Create(context.getLLVMContext(), "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(context.getLLVMContext(), "for.body", function);
  BasicBlock* forInc = BasicBlock::Create(context.getLLVMContext(), "for.inc", function);
  BasicBlock* forEnd = BasicBlock::Create(context.getLLVMContext(), "for.end", function);
  
  scopes.pushScope();
  
  mStartStatement.generateIR(context);
  IRWriter::createBranch(context, forCond);
  
  context.setBasicBlock(forCond);
  Value* conditionValue = mConditionStatement.generateIR(context);
  IRWriter::createConditionalBranch(context, forBody, forEnd, conditionValue);
  
  scopes.setBreakToBlock(forEnd);
  scopes.setContinueToBlock(forInc);
  context.setBasicBlock(forBody);
  mBodyStatement.generateIR(context);
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);

  IRWriter::createBranch(context, forInc);
  context.setBasicBlock(forInc);
  mIncrementExpression.generateIR(context);

  IRWriter::createBranch(context, forCond);
  context.setBasicBlock(forEnd);
  
  scopes.popScope(context);
  
  return conditionValue;
}
