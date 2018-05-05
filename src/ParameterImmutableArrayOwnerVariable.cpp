//
//  ParameterImmutableArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterImmutableArrayOwnerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterImmutableArrayOwnerVariable::
ParameterImmutableArrayOwnerVariable(string name,
                                     const ImmutableArrayOwnerType* immutableArrayOwnerType,
                                     Value* valueStore,
                                     int line) :
mName(name),
mImmutableArrayOwnerType(immutableArrayOwnerType),
mValueStore(valueStore),
mLine(line) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
}

ParameterImmutableArrayOwnerVariable::~ParameterImmutableArrayOwnerVariable() {
}

string ParameterImmutableArrayOwnerVariable::getName() const {
  return mName;
}

const ImmutableArrayOwnerType* ParameterImmutableArrayOwnerVariable::getType() const {
  return mImmutableArrayOwnerType;
}

bool ParameterImmutableArrayOwnerVariable::isField() const {
  return false;
}

bool ParameterImmutableArrayOwnerVariable::isSystem() const {
  return false;
}

int ParameterImmutableArrayOwnerVariable::getLine() const {
  return mLine;
}

Value* ParameterImmutableArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                                  int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* ParameterImmutableArrayOwnerVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  return mValueStore;
}

Value* ParameterImmutableArrayOwnerVariable::
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

void ParameterImmutableArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  Value* null = ConstantPointerNull::get(mImmutableArrayOwnerType->getLLVMType(context));
  IRWriter::newStoreInst(context, null, mValueStore);
}

void ParameterImmutableArrayOwnerVariable::free(IRGenerationContext& context, int line) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mImmutableArrayOwnerType->free(context, valueLoaded, line);
}
