//
//  IfStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
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
  checkUnreachable(context, mCondition->getLine());
  IExpression::checkForUndefined(context, mCondition);

  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);
  
  Value* conditionValue = mCondition->generateIR(context, PrimitiveTypes::VOID);
  Value* castConditionValue = AutoCast::maybeCast(context,
                                                  mCondition->getType(context),
                                                  conditionValue,
                                                  PrimitiveTypes::BOOLEAN,
                                                  mCondition->getLine());
  IRWriter::createConditionalBranch(context, ifThen, ifEnd, castConditionValue);
  
  context.setBasicBlock(ifThen);
  mThenStatement->generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifEnd);
}

