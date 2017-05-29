//
//  SwitchStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CaseStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/SwitchStatement.hpp"

using namespace llvm;
using namespace wisey;

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
    IRWriter::createBranch(context, caseStatement->isFallThrough() ? nextCaseBlock : switchEpilog);
    
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
  IRWriter::createBranch(context, switchEpilog);
}
