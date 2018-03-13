//
//  ParameterReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterReferenceVariable::ParameterReferenceVariable(string name,
                                                       const IObjectType* type,
                                                       Value* value) :
mName(name), mType(type), mValue(value) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isStructTy());
}

ParameterReferenceVariable::~ParameterReferenceVariable() {
}

string ParameterReferenceVariable::getName() const {
  return mName;
}

const IObjectType* ParameterReferenceVariable::getType() const {
  return mType;
}

bool ParameterReferenceVariable::isField() const {
  return false;
}

bool ParameterReferenceVariable::isSystem() const {
  return false;
}

Value* ParameterReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression,
                                                        vector<const IExpression*> arrayIndices,
                                                        int line) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  mType->decrementReferenceCount(context, mValue);
}
