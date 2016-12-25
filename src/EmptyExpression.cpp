//
//  EmptyExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/EmptyExpression.hpp"

using namespace llvm;
using namespace yazyk;

const EmptyExpression EmptyExpression::EMPTY_EXPRESSION = EmptyExpression();

Value* EmptyExpression::generateIR(IRGenerationContext& context) const {
  return NULL;
}
