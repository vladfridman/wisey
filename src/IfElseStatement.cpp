//
//  IfElseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IfElseStatement.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IfElseStatement::~IfElseStatement() {
  delete mCondition;
  delete mThenStatement;
  delete mElseStatement;
}
Value* IfElseStatement::generateIR(IRGenerationContext& context) const {
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifElse = BasicBlock::Create(context.getLLVMContext(), "if.else", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);

  Value* conditionValue = mCondition->generateIR(context);
  IRWriter::createConditionalBranch(context, ifThen, ifElse, conditionValue);
  
  context.setBasicBlock(ifThen);
  map<string, IVariable*> clearedVariablesBefore = context.getScopes().getClearedVariables();
  mThenStatement->generateIR(context);
  map<string, IVariable*> clearedVariablesAfter = context.getScopes().getClearedVariables();
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifElse);
  context.getScopes().setClearedVariables(clearedVariablesBefore);
  mElseStatement->generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  for (map<string, IVariable*>::iterator iterator = clearedVariablesAfter.begin();
       iterator != clearedVariablesAfter.end();
       iterator++) {
    context.getScopes().clearVariable(context, iterator->first);
  }
  
  context.setBasicBlock(ifEnd);

  return conditionValue;
}

