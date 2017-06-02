//
//  ExpressionStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExpressionStatement.hpp"

using namespace llvm;
using namespace wisey;

ExpressionStatement::~ExpressionStatement() {
  delete mExpression;
}

Value* ExpressionStatement::generateIR(IRGenerationContext& context) const {
  return mExpression->generateIR(context);
}
