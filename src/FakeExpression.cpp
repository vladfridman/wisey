//
//  FakeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IVariable* FakeExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* FakeExpression::generateIR(IRGenerationContext& context) const {
  return mValue;
}

const IType* FakeExpression::getType(IRGenerationContext& context) const {
  return mType;
}

void FakeExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a fake epxression");
  exit(1);
}

bool FakeExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

bool FakeExpression::isConstant() const {
  return false;
}

void FakeExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
}
