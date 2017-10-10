//
//  HeapOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapOwnerVariable::getName() const {
  return mName;
}

const IObjectOwnerType* HeapOwnerVariable::getType() const {
  return mType;
}

Value* HeapOwnerVariable::getValue() const {
  return mValue;
}

Value* HeapOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                               string llvmVariableName) const {
  if (!mIsInitialized) {
    Log::e("Variable '" + mName + "' is used before it is initialized");
    exit(1);
  }
  
  return mValue;
}

Value* HeapOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
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

void HeapOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValue);
  mIsInitialized = true;
}

void HeapOwnerVariable::free(IRGenerationContext& context) const {
  mType->free(context, mValue);
}

bool HeapOwnerVariable::existsInOuterScope() const {
  return false;
}
