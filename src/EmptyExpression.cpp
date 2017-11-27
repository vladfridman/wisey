//
//  EmptyExpression.cpp
//  Wisey
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

IVariable* EmptyExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* EmptyExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  return NULL;
}

const IType* EmptyExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::VOID_TYPE;
}

bool EmptyExpression::isConstant() const {
  return false;
}

void EmptyExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
}

