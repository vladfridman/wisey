//
//  DoWhileStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/DoStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

DoStatement::DoStatement(IStatement* statement, IExpression* conditionExpression, int line)
: mStatement(statement), mConditionExpression(conditionExpression), mLine(line) { }

DoStatement::~DoStatement() {
  delete mStatement;
  delete mConditionExpression;
}

void DoStatement::generateIR(IRGenerationContext& context) const {
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
  Value* conditionValue = mConditionExpression->generateIR(context, PrimitiveTypes::VOID);
  IRWriter::createConditionalBranch(context, doBody, doEnd, conditionValue);
  
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);
  
  context.setBasicBlock(doEnd);
}

int DoStatement::getLine() const {
  return mLine;
}
