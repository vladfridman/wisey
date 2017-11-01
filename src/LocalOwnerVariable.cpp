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

LocalOwnerVariable::LocalOwnerVariable(string name, const IObjectOwnerType* type, Value* value) :
mName(name), mType(type), mValue(value), mIsInitialized(false) {
  assert(value->getType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->isPointerTy());
  assert(value->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
}

LocalOwnerVariable::~LocalOwnerVariable() {
}

string LocalOwnerVariable::getName() const {
  return mName;
}

const IObjectOwnerType* LocalOwnerVariable::getType() const {
  return mType;
}

Value* LocalOwnerVariable::getValue() const {
  return mValue;
}

Value* LocalOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                string llvmVariableName) const {
  if (!mIsInitialized) {
    Log::e("Variable '" + mName + "' is used before it is initialized");
    exit(1);
  }
  
  return IRWriter::newLoadInst(context, mValue, "");
}

Value* LocalOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  free(context);
  
  IRWriter::newStoreInst(context, newValue, mValue);
  
  mIsInitialized = true;
  
  return mValue;
}

void LocalOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValue);
  mIsInitialized = true;
}

void LocalOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValue, "");
  mType->free(context, valueLoaded);
}

bool LocalOwnerVariable::existsInOuterScope() const {
  return false;
}
