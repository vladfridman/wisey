//
//  ParameterImmutableArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterImmutableArrayReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterImmutableArrayReferenceVariable::
ParameterImmutableArrayReferenceVariable(string name,
                                         const ImmutableArrayType* immutableArrayType,
                                         Value* value) :
mName(name), mImmutableArrayType(immutableArrayType), mValue(value) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isStructTy());
}

ParameterImmutableArrayReferenceVariable::~ParameterImmutableArrayReferenceVariable() {
}

string ParameterImmutableArrayReferenceVariable::getName() const {
  return mName;
}

const ImmutableArrayType* ParameterImmutableArrayReferenceVariable::getType() const {
  return mImmutableArrayType;
}

bool ParameterImmutableArrayReferenceVariable::isField() const {
  return false;
}

bool ParameterImmutableArrayReferenceVariable::isSystem() const {
  return false;
}

Value* ParameterImmutableArrayReferenceVariable::
generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterImmutableArrayReferenceVariable::
generateIdentifierReferenceIR(IRGenerationContext& context) const {
  assert(false);
}

Value* ParameterImmutableArrayReferenceVariable::
generateAssignmentIR(IRGenerationContext& context,
                     IExpression* assignToExpression,
                     vector<const IExpression*> arrayIndices,
                     int line) {
  if (!arrayIndices.size()) {
    context.reportError(line, "Assignment to method parameters is not allowed");
    exit(1);
  }

  context.reportError(line, "Attempting to change a value in an immutable array");
  exit(1);
}

void ParameterImmutableArrayReferenceVariable::
decrementReferenceCounter(IRGenerationContext& context) const {
  mImmutableArrayType->decrementReferenceCount(context, mValue);
}
