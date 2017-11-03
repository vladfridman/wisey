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

Value* ParameterReferenceVariable::getValue() const {
  return mValue;
}

Value* ParameterReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                        string llvmVariableName) const {
  return mValue;
}

Value* ParameterReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                        IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterReferenceVariable::setToNull(IRGenerationContext& context) {
  return;
}

void ParameterReferenceVariable::free(IRGenerationContext& context) const {
  return;
}

bool ParameterReferenceVariable::existsInOuterScope() const {
  return true;
}
