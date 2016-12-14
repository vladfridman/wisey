//
//  LogicalOrExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/codegen.hpp"
#include "yazyk/LogicalOrExpression.hpp"

using namespace llvm;
using namespace yazyk;

Value *LogicalOrExpression::generateIR(IRGenerationContext& context) {
  Value* leftValue = mLeftExpression.generateIR(context);
  BasicBlock* entryBlock = context.currentBlock();
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* bblockRhs = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock* bblockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  BranchInst::Create(bblockEnd, bblockRhs, leftValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value* rightValue = mRightExpression.generateIR(context);
  BasicBlock* lastRightBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = PHINode::Create(type, 0, "lor", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getTrue(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rightValue, lastRightBlock);
  
  return phiNode;
}
