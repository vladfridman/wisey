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

BinaryOperator* IRWriter::createBinaryOperator(IRGenerationContext& context,
                                               Instruction::BinaryOps instruction,
                                               Value* leftValue,
                                               Value* rightValue,
                                               string llvmVariableName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return BinaryOperator::Create(instruction, leftValue, rightValue, llvmVariableName, currentBlock);
}

CallInst* IRWriter::createCallInst(IRGenerationContext& context,
                                   Function* function,
                                   vector<Value*> arguments,
                                   string resultName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return CallInst::Create(function, arguments, resultName, currentBlock);
}
