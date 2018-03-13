//
//  ParameterSystemReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Log.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterSystemReferenceVariable::ParameterSystemReferenceVariable(string name,
                                                                   const IObjectType* type,
                                                                   Value* value) :
mName(name), mType(type), mValue(value) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isStructTy());
}

ParameterSystemReferenceVariable::~ParameterSystemReferenceVariable() {
}

string ParameterSystemReferenceVariable::getName() const {
  return mName;
}

const IObjectType* ParameterSystemReferenceVariable::getType() const {
  return mType;
}

bool ParameterSystemReferenceVariable::isSystem() const {
  return true;
}

Value* ParameterSystemReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValue;
}

Value* ParameterSystemReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                              IExpression* assignToExpression,
                                                              vector<const IExpression*>
                                                              arrayIndices,
                                                              int line) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterSystemReferenceVariable::decrementReferenceCounter(IRGenerationContext&
                                                                 context) const {
  mType->decrementReferenceCount(context, mValue);
}
