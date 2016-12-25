//
//  ConditionalExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/ConditionalExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value *ConditionalExpression::generateIR(IRGenerationContext& context) const {
  Value* conditionValue = mConditionExpression.generateIR(context);
  
  Function* function = context.currentBlock()->getParent();
  
  BasicBlock* blockCondTrue = BasicBlock::Create(context.getLLVMContext(), "cond.true", function);
  BasicBlock* blockCondFalse = BasicBlock::Create(context.getLLVMContext(), "cond.false", function);
  BasicBlock* blockCondEnd = BasicBlock::Create(context.getLLVMContext(), "cond.end", function);
  BranchInst::Create(blockCondTrue, blockCondFalse, conditionValue, context.currentBlock());
  
  context.replaceBlock(blockCondTrue);
  Value * condTrueValue = mConditionTrueExpression.generateIR(context);
  Type * condTrueResultType = condTrueValue->getType();
  BasicBlock * lastBlock = context.currentBlock();
  BranchInst::Create(blockCondEnd, context.currentBlock());

  context.replaceBlock(blockCondFalse);
  Value * condFalseValue = mConditionFalseExpression.generateIR(context);
  Type * condFalseResultType = condTrueValue->getType();
  lastBlock = context.currentBlock();
  BranchInst::Create(blockCondEnd, context.currentBlock());

  if (condTrueResultType != condFalseResultType) {
    Log::e("Results of different type in a conditional expresion!");
  }

  context.replaceBlock(blockCondEnd);
  PHINode * phiNode = PHINode::Create(condTrueResultType,
                                      0,
                                      "cond",
                                      context.currentBlock());
  phiNode->addIncoming(condTrueValue, blockCondTrue);
  phiNode->addIncoming(condFalseValue, blockCondFalse);
  
  return phiNode;
}

