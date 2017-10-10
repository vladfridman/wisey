//
//  HeapReferenceMethodParameter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/HeapReferenceMethodParameter.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapReferenceMethodParameter::getName() const {
  return mName;
}

const IObjectType* HeapReferenceMethodParameter::getType() const {
  return mType;
}

Value* HeapReferenceMethodParameter::getValue() const {
  return mValue;
}

Value* HeapReferenceMethodParameter::generateIdentifierIR(IRGenerationContext& context,
                                                          string llvmVariableName) const {
  return mValue;
}

Value* HeapReferenceMethodParameter::generateAssignmentIR(IRGenerationContext& context,
                                                          IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void HeapReferenceMethodParameter::setToNull(IRGenerationContext& context) {
  return;
}

void HeapReferenceMethodParameter::free(IRGenerationContext& context) const {
  return;
}

bool HeapReferenceMethodParameter::existsInOuterScope() const {
  return true;
}
