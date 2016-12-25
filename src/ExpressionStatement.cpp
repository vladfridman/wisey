//
//  ExpressionStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ExpressionStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* ExpressionStatement::generateIR(IRGenerationContext& context) const {
  return expression.generateIR(context);
}
