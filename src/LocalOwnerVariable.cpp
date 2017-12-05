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
                                       const IObjectOwnerType* type,
                                       Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
}

LocalOwnerVariable::~LocalOwnerVariable() {
}

string LocalOwnerVariable::getName() const {
  return mName;
}

const IObjectOwnerType* LocalOwnerVariable::getType() const {
  return mType;
}

Value* LocalOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  if (!mIsInitialized) {
    Log::e("Variable '" + mName + "' is used before it is initialized");
    exit(1);
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                int line) {
  Composer::pushCallStack(context, line);
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  free(context);
  
  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  mIsInitialized = true;
  
  Composer::popCallStack(context);
  
  return newValue;
}

void LocalOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
  mIsInitialized = true;
}

void LocalOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded);
}

