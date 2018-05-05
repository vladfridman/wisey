//
//  HeapVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalReferenceVariable::LocalReferenceVariable(string name,
                                               const IReferenceType* type,
                                               Value* valueStore,
                                               int line) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false), mLine(line) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  if (!type->isNative()) {
    assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
  }
}

LocalReferenceVariable::~LocalReferenceVariable() {
}

string LocalReferenceVariable::getName() const {
  return mName;
}

const IReferenceType* LocalReferenceVariable::getType() const {
  return mType;
}

bool LocalReferenceVariable::isField() const {
  return false;
}

bool LocalReferenceVariable::isSystem() const {
  return false;
}

int LocalReferenceVariable::getLine() const {
  return mLine;
}

Value* LocalReferenceVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  if (!mIsInitialized) {
    context.reportError(line, "Variable '" + mName + "' is used before it is initialized");
    throw 1;
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                             int line) const {
  return mValueStore;
}

Value* LocalReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  Composer::setLineNumber(context, line);
  
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  decrementReferenceCounter(context);
  mType->incrementReferenceCount(context, newValue);
  IRWriter::newStoreInst(context, newValue, mValueStore);

  mIsInitialized = true;

  return newValue;
}

void LocalReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mType->decrementReferenceCount(context, value);
}

void LocalReferenceVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
  mIsInitialized = true;
}
