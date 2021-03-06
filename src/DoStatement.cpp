//
//  DoWhileStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "AutoCast.hpp"
#include "DoStatement.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

DoStatement::DoStatement(IStatement* statement, IExpression* conditionExpression)
: mStatement(statement), mConditionExpression(conditionExpression) { }

DoStatement::~DoStatement() {
  delete mStatement;
  delete mConditionExpression;
}

void DoStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mConditionExpression->getLine());
  
  IExpression::checkForUndefined(context, mConditionExpression);

  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* doCond = BasicBlock::Create(context.getLLVMContext(), "do.cond", function);
  BasicBlock* doBody = BasicBlock::Create(context.getLLVMContext(), "do.body", function);
  BasicBlock* doEnd = BasicBlock::Create(context.getLLVMContext(), "do.end", function);
  
  IRWriter::createBranch(context, doBody);
  
  context.setBasicBlock(doBody);
  scopes.setBreakToBlock(doEnd);
  scopes.setContinueToBlock(doBody);
  mStatement->generateIR(context);
  IRWriter::createBranch(context, doCond);
  
  context.setBasicBlock(doCond);
  Value* conditionValue = mConditionExpression->generateIR(context, PrimitiveTypes::BOOLEAN);
  Value* conditionValueCast = AutoCast::maybeCast(context,
                                                  mConditionExpression->getType(context),
                                                  conditionValue,
                                                  PrimitiveTypes::BOOLEAN,
                                                  mConditionExpression->getLine());
  IRWriter::createConditionalBranch(context, doBody, doEnd, conditionValueCast);
  
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);
  
  context.setBasicBlock(doEnd);
}
