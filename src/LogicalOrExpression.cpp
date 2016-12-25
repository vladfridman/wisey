//
//  LogicalOrExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LogicalOrExpression.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

using namespace llvm;
using namespace yazyk;

Value *LogicalOrExpression::generateIR(IRGenerationContext& context) const {
  Value* leftValue = mLeftExpression.generateIR(context);
  BasicBlock* entryBlock = context.currentBlock();
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  BranchInst::Create(basicBlockEnd, basicBlockRight, leftValue, context.currentBlock());
  
  context.replaceBlock(basicBlockRight);
  Value* rightValue = mRightExpression.generateIR(context);
  BasicBlock* lastRightBlock = context.currentBlock();
  BranchInst::Create(basicBlockEnd, context.currentBlock());
  
  context.replaceBlock(basicBlockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = PHINode::Create(type, 0, "lor", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getTrue(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rightValue, lastRightBlock);
  
  return phiNode;
}
