//
//  ParameterReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "ParameterReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterReferenceVariable::ParameterReferenceVariable(string name,
                                                       const IReferenceType* type,
                                                       Value* value,
                                                       int line) :
mName(name), mType(type), mValue(value), mLine(line) {
  assert(value->getType()->isPointerTy());
  if (!type->isNative()) {
    assert(value->getType()->getPointerElementType()->isStructTy());
  }
}

ParameterReferenceVariable::~ParameterReferenceVariable() {
}

string ParameterReferenceVariable::getName() const {
  return mName;
}

const IReferenceType* ParameterReferenceVariable::getType() const {
  return mType;
}

bool ParameterReferenceVariable::isField() const {
  return false;
}

bool ParameterReferenceVariable::isStatic() const {
  return false;
}

int ParameterReferenceVariable::getLine() const {
  return mLine;
}

Value* ParameterReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                        int line) const {
  return mValue;
}

Value* ParameterReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                 int line) const {
  assert(false);
}

Value* ParameterReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression,
                                                        vector<const IExpression*> arrayIndices,
                                                        int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}

void ParameterReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  mType->decrementReferenceCount(context, mValue);
}
