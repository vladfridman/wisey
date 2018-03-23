//
//  LocalSystemReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalSystemReferenceVariable::LocalSystemReferenceVariable(string name,
                                                           const IObjectType* type,
                                                           Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
}

LocalSystemReferenceVariable::~LocalSystemReferenceVariable() {
}

string LocalSystemReferenceVariable::getName() const {
  return mName;
}

const IObjectType* LocalSystemReferenceVariable::getType() const {
  return mType;
}

bool LocalSystemReferenceVariable::isField() const {
  return false;
}

bool LocalSystemReferenceVariable::isSystem() const {
  return true;
}

Value* LocalSystemReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  if (!mIsInitialized) {
    Log::e("System variable '" + mName + "' is not initialized");
    exit(1);
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalSystemReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                   context) const {
  return mValueStore;
}

Value* LocalSystemReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                          IExpression* assignToExpression,
                                                          vector<const IExpression*> arrayIndices,
                                                          int line) {
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  mIsInitialized = true;
  
  return newValue;
}

void LocalSystemReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mType->decrementReferenceCount(context, value);
}
