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
using namespace std;
using namespace wisey;

FakeExpression::FakeExpression(llvm::Value* value, const IType* type) :
mValue(value), mType(type) {
}

FakeExpression::~FakeExpression() { }

IVariable* FakeExpression::getVariable(IRGenerationContext& context,
                                       vector<const IExpression*>& arrayIndices) const {
  return NULL;
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

void FakeExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
}
