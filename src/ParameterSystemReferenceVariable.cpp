//
//  ParameterSystemReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterSystemReferenceVariable::ParameterSystemReferenceVariable(string name,
                                                                   const IObjectType* type,
                                                                   Value* value,
                                                                   int line) :
mName(name), mType(type), mValue(value), mLine(line) {
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

bool ParameterSystemReferenceVariable::isField() const {
  return false;
}

bool ParameterSystemReferenceVariable::isSystem() const {
  return true;
}

int ParameterSystemReferenceVariable::getLine() const {
  return mLine;
}

Value* ParameterSystemReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                              int line) const {
  return mValue;
}

Value* ParameterSystemReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                       int line) const {
  assert(false);
}

Value* ParameterSystemReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                              IExpression* assignToExpression,
                                                              vector<const IExpression*>
                                                              arrayIndices,
                                                              int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}

void ParameterSystemReferenceVariable::decrementReferenceCounter(IRGenerationContext&
                                                                 context) const {
  mType->decrementReferenceCount(context, mValue);
}
