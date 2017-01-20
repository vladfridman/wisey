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
  context.getScopes().setBreakToBlock(switchEpilog);
  BasicBlock* switchDefault = mSwitchCases.defaultStatement != NULL
    ? BasicBlock::Create(llvmContext, "sw.default", function) : NULL;
  
  Value* conditionValue = mCondition.generateIR(context);
  SwitchInst* switchInstruction =
    SwitchInst::Create(conditionValue,
                       switchDefault != NULL ? switchDefault : switchEpilog,
                       (unsigned int) mSwitchCases.caseStatements.size(),
                       context.getBasicBlock());
  
  generateCasesIR(context, switchDefault, switchEpilog, switchInstruction);
  generateDefaultCaseIR(context, switchDefault, switchEpilog);
  
  context.getScopes().setBreakToBlock(NULL);
  context.setBasicBlock(switchEpilog);
  
  return conditionValue;
}

void SwitchStatement::generateCasesIR(IRGenerationContext& context,
                                      BasicBlock* switchDefault,
                                      BasicBlock* switchEpilog,
                                      SwitchInst* switchInstruction) const {
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* nextCaseBlock = mSwitchCases.caseStatements.size() > 0
    ? BasicBlock::Create(context.getLLVMContext(), "sw.bb", function) : NULL;
  BasicBlock* currentCaseBlock = NULL;
  BasicBlock* finalCaseBlock = switchDefault != NULL ? switchDefault : switchEpilog;
  
  std::vector<CaseStatement*>::const_iterator iterator;
  for (iterator = mSwitchCases.caseStatements.begin();
       iterator != mSwitchCases.caseStatements.end();
       iterator++) {
    CaseStatement* caseStatement = *iterator;
    
    currentCaseBlock = nextCaseBlock;
    nextCaseBlock = iterator + 1 != mSwitchCases.caseStatements.end()
      ? BasicBlock::Create(context.getLLVMContext(), "sw.bb", function) : finalCaseBlock;
    
    context.setBasicBlock(currentCaseBlock);
    caseStatement->generateIR(context);
    SafeBranch::newBranch(caseStatement->isFallThrough() ? nextCaseBlock : switchEpilog, context);
    
    switchInstruction->addCase(caseStatement->getExpressionValue(context), currentCaseBlock);
  }
  
}

void SwitchStatement::generateDefaultCaseIR(IRGenerationContext& context,
                                            BasicBlock* switchDefault,
                                            BasicBlock* switchEpilog) const {
  if (switchDefault == NULL) {
    return;
  }
  
  context.setBasicBlock(switchDefault);
  mSwitchCases.defaultStatement->generateIR(context);
  SafeBranch::newBranch(switchEpilog, context);
}
