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
  
  BasicBlock* switchEpilog = BasicBlock::Create(context.getLLVMContext(), "sw.epilog", function);
  context.setBreakToBlock(switchEpilog);
  
  Value* conditionValue = mCondition.generateIR(context);
  SwitchInst* switchInst = SwitchInst::Create(conditionValue,
                                              switchEpilog,
                                              mCaseStatementList.size(),
                                              context.getBasicBlock());
  std::vector<CaseStatement*>::const_iterator iterator;
  for (iterator = mCaseStatementList.begin();
       iterator != mCaseStatementList.end();
       iterator++) {
    CaseStatement* caseStatement = *iterator;
    BasicBlock* block = BasicBlock::Create(context.getLLVMContext(), "sw.bb", function);
    
    context.setBasicBlock(block);
    caseStatement->generateIR(context);
    SafeBranch::newBranch(context.getBreakToBlock(), context);

    switchInst->addCase(caseStatement->getExpressionValue(context), block);
  }
  
  context.setBreakToBlock(NULL);
  
  context.setBasicBlock(switchEpilog);
  
  return conditionValue;
}

