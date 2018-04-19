//
//  FakeExpressionWithCleanup.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FakeExpressionWithCleanup::FakeExpressionWithCleanup(llvm::Value* value, const IType* type) :
mValue(value), mType(type) {
}

FakeExpressionWithCleanup::~FakeExpressionWithCleanup() {
  delete mType;
}

int FakeExpressionWithCleanup::getLine() const {
  return 0;
}

Value* FakeExpressionWithCleanup::generateIR(IRGenerationContext& context,
                                             const IType* assignToType) const {
  return mValue;
}

const IType* FakeExpressionWithCleanup::getType(IRGenerationContext& context) const {
  return mType;
}

bool FakeExpressionWithCleanup::isConstant() const {
  return false;
}

bool FakeExpressionWithCleanup::isAssignable() const {
  return false;
}

void FakeExpressionWithCleanup::printToStream(IRGenerationContext& context,
                                              std::iostream& stream) const {
}

