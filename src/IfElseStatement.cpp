//
//  IfElseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "AutoCast.hpp"
#include "IfElseStatement.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IfElseStatement::IfElseStatement(IExpression* condition,
                                 CompoundStatement* thenStatement,
                                 IStatement* elseStatement) :
mCondition(condition),
mThenStatement(thenStatement),
mElseStatement(elseStatement) { }

IfElseStatement::~IfElseStatement() {
  delete mCondition;
  delete mThenStatement;
  delete mElseStatement;
}

void IfElseStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mCondition->getLine());
  IExpression::checkForUndefined(context, mCondition);

  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifElse = BasicBlock::Create(context.getLLVMContext(), "if.else", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);

  Value* conditionValue = mCondition->generateIR(context, PrimitiveTypes::VOID);
  Value* castConditionValue = AutoCast::maybeCast(context,
                                                  mCondition->getType(context),
                                                  conditionValue,
                                                  PrimitiveTypes::BOOLEAN,
                                                  mCondition->getLine());
  IRWriter::createConditionalBranch(context, ifThen, ifElse, castConditionValue);
  
  context.setBasicBlock(ifThen);
  mThenStatement->generateIR(context);
  Value* createdIfBranch = IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifElse);
  mElseStatement->generateIR(context);
  Value* createdElseBranch = IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifEnd);
  if (!createdIfBranch && !createdElseBranch) {
    IRWriter::newUnreachableInst(context);
  }
}

