//
//  LogicalAndExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/codegen.hpp"
#include "yazyk/LogicalAndExpression.hpp"

using namespace llvm;
using namespace yazyk;

Value* LogicalAndExpression::generateIR(IRGenerationContext& context) {
  Value* leftValue = mLeftExpression.generateIR(context);
  BasicBlock * entryBlock = context.currentBlock();
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock *bblockRhs = BasicBlock::Create(context.getLLVMContext(), "land.rhs", function);
  BasicBlock *bblockEnd = BasicBlock::Create(context.getLLVMContext(), "land.end", function);
  BranchInst::Create(bblockRhs, bblockEnd, leftValue, context.currentBlock());
  
  context.replaceBlock(bblockRhs);
  Value* rightValue = mRightExpression.generateIR(context);
  BasicBlock * lastRhsBlock = context.currentBlock();
  BranchInst::Create(bblockEnd, context.currentBlock());
  
  context.replaceBlock(bblockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = PHINode::Create(type, 0, "land", context.currentBlock());
  phiNode->addIncoming(ConstantInt::getFalse(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rightValue, lastRhsBlock);
  
  return phiNode;
}
