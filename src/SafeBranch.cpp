//
//  SafeBranch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

BranchInst* SafeBranch::newBranch(BasicBlock* toBlock, IRGenerationContext& context) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->size() > 0 && TerminatorInst::classof(&currentBlock->back())) {
    return NULL;
  }
  
  return BranchInst::Create(toBlock, context.getBasicBlock());
}

BranchInst* SafeBranch::newConditionalBranch(BasicBlock* ifTrueBlock,
                                             BasicBlock* ifFalseBlock,
                                             Value* condition,
                                             IRGenerationContext& context) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->size() > 0 && TerminatorInst::classof(&currentBlock->back())) {
    return NULL;
  }
  
  return BranchInst::Create(ifTrueBlock, ifFalseBlock, condition, context.getBasicBlock());
}
