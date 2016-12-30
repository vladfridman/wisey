//
//  LogicalOrExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LogicalOrExpression.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value *LogicalOrExpression::generateIR(IRGenerationContext& context) const {
  Value* leftValue = mLeftExpression.generateIR(context);
  BasicBlock* entryBlock = context.getBasicBlock();
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* basicBlockRight = BasicBlock::Create(context.getLLVMContext(), "lor.rhs", function);
  BasicBlock* basicBlockEnd = BasicBlock::Create(context.getLLVMContext(), "lor.end", function);
  SafeBranch::newConditionalBranch(basicBlockEnd, basicBlockRight, leftValue, context);
  
  context.setBasicBlock(basicBlockRight);
  Value* rightValue = mRightExpression.generateIR(context);
  BasicBlock* lastRightBlock = context.getBasicBlock();
  SafeBranch::newBranch(basicBlockEnd, context);
  
  context.setBasicBlock(basicBlockEnd);
  Type* type = Type::getInt1Ty(context.getLLVMContext());
  PHINode* phiNode = PHINode::Create(type, 0, "lor", context.getBasicBlock());
  phiNode->addIncoming(ConstantInt::getTrue(context.getLLVMContext()), entryBlock);
  phiNode->addIncoming(rightValue, lastRightBlock);
  
  return phiNode;
}
