//
//  EmptyExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/EmptyExpression.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

const EmptyExpression EmptyExpression::EMPTY_EXPRESSION = EmptyExpression();

Value* EmptyExpression::generateIR(IRGenerationContext& context) const {
  return NULL;
}

IType* EmptyExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::VOID_TYPE;
}

void EmptyExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an empty epxression, it is not a heap pointer");
  exit(1);
}
