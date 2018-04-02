//
//  LocalOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalOwnerVariable::LocalOwnerVariable(string name,
                                       const IOwnerType* type,
                                       Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  if (!type->isNative()) {
    assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
  }
}

LocalOwnerVariable::~LocalOwnerVariable() {
}

string LocalOwnerVariable::getName() const {
  return mName;
}

const IOwnerType* LocalOwnerVariable::getType() const {
  return mType;
}

bool LocalOwnerVariable::isField() const {
  return false;
}

bool LocalOwnerVariable::isSystem() const {
  return false;
}

Value* LocalOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  if (!mIsInitialized) {
    Log::e("Variable '" + mName + "' is used before it is initialized");
    exit(1);
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return mValueStore;
}

Value* LocalOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                vector<const IExpression*> arrayIndices,
                                                int line) {
  Composer::pushCallStack(context, line);
  
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  free(context);
  
  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  mIsInitialized = true;
  
  Composer::popCallStack(context);
  
  return newValue;
}

void LocalOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
  mIsInitialized = true;
}

void LocalOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded);
}

