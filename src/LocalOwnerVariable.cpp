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
  
  return mValue;
}

Value* LocalOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  free(context);
  
  Value* newValueLoaded = IRWriter::newLoadInst(context, newValue, "");
  IRWriter::newStoreInst(context, newValueLoaded, mValue);
  
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
  mType->free(context, mValue);
}

bool LocalOwnerVariable::existsInOuterScope() const {
  return false;
}