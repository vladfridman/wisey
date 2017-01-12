//
//  SwitchStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/CaseStatement.hpp"
#include "yazyk/SwitchStatement.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value* SwitchStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* switchEpilog = BasicBlock::Create(llvmContext, "sw.epilog", function);
  context.setBreakToBlock(switchEpilog);
  BasicBlock* switchDefault = mSwitchCases.defaultStatement != NULL
    ? BasicBlock::Create(llvmContext, "sw.default", function) : NULL;
  
  Value* conditionValue = mCondition.generateIR(context);
  SwitchInst* switchInst =
    SwitchInst::Create(conditionValue,
                       switchDefault != NULL ? switchDefault : switchEpilog,
                       mSwitchCases.caseStatements.size(),
                       context.getBasicBlock());
  
  std::vector<CaseStatement*>::const_iterator iterator;
  for (iterator = mSwitchCases.caseStatements.begin();
       iterator != mSwitchCases.caseStatements.end();
       iterator++) {
    CaseStatement* caseStatement = *iterator;
    BasicBlock* block = BasicBlock::Create(context.getLLVMContext(), "sw.bb", function);
    
    context.setBasicBlock(block);
    caseStatement->generateIR(context);
    SafeBranch::newBranch(switchEpilog, context);

    switchInst->addCase(caseStatement->getExpressionValue(context), block);
  }
  
  if (switchDefault != NULL) {
    context.setBasicBlock(switchDefault);
    mSwitchCases.defaultStatement->generateIR(context);
    SafeBranch::newBranch(switchEpilog, context);
  }
  
  context.setBreakToBlock(NULL);
  context.setBasicBlock(switchEpilog);
  
  return conditionValue;
}

