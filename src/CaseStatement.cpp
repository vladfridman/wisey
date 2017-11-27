//
//  CaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CaseStatement.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

CaseStatement::~CaseStatement() {
  delete mExpression;
  delete mBlock;
}

Value* CaseStatement::generateIR(IRGenerationContext& context) const {
  return mBlock->generateIR(context);
}

bool CaseStatement::isFallThrough() const {
  return mIsFallThrough;
}

ConstantInt* CaseStatement::getExpressionValue(IRGenerationContext& context) const {
  Value* value = mExpression->generateIR(context, IR_GENERATION_NORMAL);
  if (!ConstantInt::classof(value)) {
    Log::e("Case expression should be an integer constant");
    exit(1);
  }
  
  return (ConstantInt*) value;
}

CaseStatement* CaseStatement::newCaseStatement(IExpression* expression, Block* block) {
  return new CaseStatement(expression, block, false);
}

CaseStatement* CaseStatement::newCaseStatementWithFallThrough(IExpression* expression,
                                                              Block* block) {
  return new CaseStatement(expression, block, true);
}
