//
//  IfElseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IfElseStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

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

Value* IfElseStatement::generateIR(IRGenerationContext& context) const {
  Function* function = context.getBasicBlock()->getParent();
  
  BasicBlock* ifThen = BasicBlock::Create(context.getLLVMContext(), "if.then", function);
  BasicBlock* ifElse = BasicBlock::Create(context.getLLVMContext(), "if.else", function);
  BasicBlock* ifEnd = BasicBlock::Create(context.getLLVMContext(), "if.end", function);

  Value* conditionValue = mCondition->generateIR(context, PrimitiveTypes::VOID_TYPE);
  IRWriter::createConditionalBranch(context, ifThen, ifElse, conditionValue);
  
  context.setBasicBlock(ifThen);
  mThenStatement->generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifElse);
  mElseStatement->generateIR(context);
  IRWriter::createBranch(context, ifEnd);
  
  context.setBasicBlock(ifEnd);

  return conditionValue;
}

