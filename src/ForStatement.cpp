//
//  ForStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ForStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

ForStatement::ForStatement(IStatement* startStatement,
                           IExpression* conditionExpression,
                           IExpression* incrementExpression,
                           IStatement* bodyStatement,
                           int line) :
mStartStatement(startStatement),
mConditionExpression(conditionExpression),
mIncrementExpression(incrementExpression),
mBodyStatement(bodyStatement),
mLine(line) { }

ForStatement::~ForStatement() {
  delete mStartStatement;
  delete mConditionExpression;
  delete mIncrementExpression;
  delete mBodyStatement;
}

void ForStatement::generateIR(IRGenerationContext& context) const {
  
  Function* function = context.getBasicBlock()->getParent();
  Scopes& scopes = context.getScopes();
  
  BasicBlock* forCond = BasicBlock::Create(context.getLLVMContext(), "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(context.getLLVMContext(), "for.body", function);
  BasicBlock* forInc = BasicBlock::Create(context.getLLVMContext(), "for.inc", function);
  BasicBlock* forEnd = BasicBlock::Create(context.getLLVMContext(), "for.end", function);
  
  scopes.pushScope();
  
  mStartStatement->generateIR(context);
  IRWriter::createBranch(context, forCond, mConditionExpression->getLine());
  
  context.setBasicBlock(forCond);
  Value* conditionValue = mConditionExpression->generateIR(context, PrimitiveTypes::VOID);
  IRWriter::createConditionalBranch(context,
                                    forBody,
                                    forEnd,
                                    conditionValue,
                                    mConditionExpression->getLine());
  
  scopes.setBreakToBlock(forEnd);
  scopes.setContinueToBlock(forInc);
  context.setBasicBlock(forBody);
  mBodyStatement->generateIR(context);
  scopes.setBreakToBlock(NULL);
  scopes.setContinueToBlock(NULL);

  if (!context.getBasicBlock()->getTerminator()) {
    IRWriter::createBranch(context, forInc, mIncrementExpression->getLine());
  }
  context.setBasicBlock(forInc);
  mIncrementExpression->generateIR(context, PrimitiveTypes::VOID);

  IRWriter::createBranch(context, forCond, mBodyStatement->getLine());
  context.setBasicBlock(forEnd);
  
  scopes.popScope(context, mLine);
}

ForStatement* ForStatement::newWithNoIncrement(IStatement* startStatement,
                                               IExpression* conditionExpression,
                                               IStatement* bodyStatement,
                                               int line) {
  return new ForStatement(startStatement,
                          conditionExpression,
                          new EmptyExpression(),
                          bodyStatement,
                          line);
}

int ForStatement::getLine() const {
  return mLine;
}
