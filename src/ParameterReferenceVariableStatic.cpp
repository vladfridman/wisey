//
//  ParameterReferenceVariableStatic.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterReferenceVariableStatic.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterReferenceVariableStatic::ParameterReferenceVariableStatic(string name,
                                                                   const IObjectType* type,
                                                                   Value* value,
                                                                   int line) :
mName(name), mType(type), mValue(value), mLine(line) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isStructTy());
}

ParameterReferenceVariableStatic::~ParameterReferenceVariableStatic() {
}

string ParameterReferenceVariableStatic::getName() const {
  return mName;
}

const IObjectType* ParameterReferenceVariableStatic::getType() const {
  return mType;
}

bool ParameterReferenceVariableStatic::isField() const {
  return false;
}

bool ParameterReferenceVariableStatic::isStatic() const {
  return true;
}

int ParameterReferenceVariableStatic::getLine() const {
  return mLine;
}

Value* ParameterReferenceVariableStatic::generateIdentifierIR(IRGenerationContext& context,
                                                              int line) const {
  return mValue;
}

Value* ParameterReferenceVariableStatic::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                       int line) const {
  assert(false);
}

Value* ParameterReferenceVariableStatic::generateAssignmentIR(IRGenerationContext& context,
                                                              IExpression* assignToExpression,
                                                              vector<const IExpression*>
                                                              arrayIndices,
                                                              int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}

void ParameterReferenceVariableStatic::decrementReferenceCounter(IRGenerationContext&
                                                                 context) const {
  mType->decrementReferenceCount(context, mValue);
}
