//
//  ParameterImmutableArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "ArrayElementAssignment.hpp"
#include "ArrayElementExpression.hpp"
#include "AutoCast.hpp"
#include "Composer.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "ParameterImmutableArrayReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterImmutableArrayReferenceVariable::
ParameterImmutableArrayReferenceVariable(string name,
                                         const ImmutableArrayType* immutableArrayType,
                                         Value* value,
                                         int line) :
mName(name), mImmutableArrayType(immutableArrayType), mValue(value), mLine(line) {
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

bool ParameterImmutableArrayReferenceVariable::isStatic() const {
  return false;
}

int ParameterImmutableArrayReferenceVariable::getLine() const {
  return mLine;
}

Value* ParameterImmutableArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                                      int line) const {
  return mValue;
}

Value* ParameterImmutableArrayReferenceVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  assert(false);
}

Value* ParameterImmutableArrayReferenceVariable::
generateAssignmentIR(IRGenerationContext& context,
                     IExpression* assignToExpression,
                     vector<const IExpression*> arrayIndices,
                     int line) {
  if (!arrayIndices.size()) {
    context.reportError(line, "Assignment to method parameters is not allowed");
    throw 1;
  }

  context.reportError(line, "Attempting to change a value in an immutable array");
  throw 1;
}

void ParameterImmutableArrayReferenceVariable::
decrementReferenceCounter(IRGenerationContext& context) const {
  mImmutableArrayType->decrementReferenceCount(context, mValue);
}
