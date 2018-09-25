//
//  FakeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "FakeExpression.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FakeExpression::FakeExpression(llvm::Value* value, const IType* type) :
mValue(value), mType(type) {
}

FakeExpression::~FakeExpression() { }

int FakeExpression::getLine() const {
  return 0;
}

Value* FakeExpression::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  return mValue;
}

const IType* FakeExpression::getType(IRGenerationContext& context) const {
  return mType;
}

bool FakeExpression::isConstant() const {
  return false;
}

bool FakeExpression::isAssignable() const {
  return false;
}

void FakeExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
}
