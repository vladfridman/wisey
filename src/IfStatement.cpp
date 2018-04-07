//
//  IfStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IfStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IfStatement::IfStatement(IExpression* condition, CompoundStatement* thenStatement) :
mCondition(condition), mThenStatement(thenStatement) { }

IfStatement::~IfStatement() {
  delete mCondition;
  delete mThenStatement;
}

void IfStatement::generateIR(IRGenerationContext& context) const {
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);
  
  Value* conditionValue = mCondition->generateIR(context, PrimitiveTypes::VOID_TYPE);
  IRWriter::createConditionalBranch(context, ifThen, ifEnd, conditionValue);
  
  context.setBasicBlock(ifThen);
  mThenStatement->generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifEnd);
}

