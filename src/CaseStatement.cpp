//
//  CaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CaseStatement.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

CaseStatement::CaseStatement(IExpression* expression, Block* block, bool isFallThrough, int line)
: mExpression(expression), mBlock(block), mIsFallThrough(isFallThrough), mLine(line) { }

CaseStatement::~CaseStatement() {
  delete mExpression;
  delete mBlock;
}

void CaseStatement::generateIR(IRGenerationContext& context) const {
  mBlock->generateIR(context);
}

bool CaseStatement::isFallThrough() const {
  return mIsFallThrough;
}

ConstantInt* CaseStatement::getExpressionValue(IRGenerationContext& context) const {
  Value* value = mExpression->generateIR(context, PrimitiveTypes::VOID);
  if (!ConstantInt::classof(value)) {
    context.reportError(mLine, "Case expression should be an integer constant");
    exit(1);
  }
  
  return (ConstantInt*) value;
}

int CaseStatement::getLine() const {
  return mLine;
}

CaseStatement* CaseStatement::newCaseStatement(IExpression* expression, Block* block, int line) {
  return new CaseStatement(expression, block, false, line);
}

CaseStatement* CaseStatement::newCaseStatementWithFallThrough(IExpression* expression,
                                                              Block* block,
                                                              int line) {
  return new CaseStatement(expression, block, true, line);
}
