//
//  IRWriter.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ReturnInst* IRWriter::createReturnInst(IRGenerationContext& context, Value* returnValue) {
  if (context.getBasicBlock()->getTerminator()) {
    return NULL;
  }
  return ReturnInst::Create(context.getLLVMContext(), returnValue, context.getBasicBlock());
}

BranchInst* IRWriter::createBranch(IRGenerationContext& context,
                                   BasicBlock* toBlock) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(toBlock, currentBlock);
}

BranchInst* IRWriter::createConditionalBranch(IRGenerationContext& context,
                                              BasicBlock* ifTrueBlock,
                                              BasicBlock* ifFalseBlock,
                                              Value* condition) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(ifTrueBlock, ifFalseBlock, condition, currentBlock);
}
