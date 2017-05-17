//
//  IfStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IfStatement.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace wisey;

Value* IfStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);
  
  Value* conditionValue = mCondition.generateIR(context);
  IRWriter::createConditionalBranch(context, ifThen, ifEnd, conditionValue);
  
  context.setBasicBlock(ifThen);
  mThenStatement.generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifEnd);
  
  return conditionValue;
}

