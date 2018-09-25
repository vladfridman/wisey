//
//  ExpressionStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "ExpressionStatement.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

ExpressionStatement::ExpressionStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

ExpressionStatement::~ExpressionStatement() {
  delete mExpression;
}

void ExpressionStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);

  mExpression->generateIR(context, PrimitiveTypes::VOID);
}
