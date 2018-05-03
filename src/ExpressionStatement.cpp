//
//  ExpressionStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExpressionStatement.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

ExpressionStatement::ExpressionStatement(IExpression* expression) : mExpression(expression) { }

ExpressionStatement::~ExpressionStatement() {
  delete mExpression;
}

void ExpressionStatement::generateIR(IRGenerationContext& context) const {
  mExpression->generateIR(context, PrimitiveTypes::VOID);
}

int ExpressionStatement::getLine() const {
  return mExpression->getLine();
}
