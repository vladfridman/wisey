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

string LocalReferenceVariable::getName() const {
  return mName;
}

const IObjectType* LocalReferenceVariable::getType() const {
  return mType;
}

Value* LocalReferenceVariable::getValue() const {
  return mValue;
}

Value* LocalReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  if (!mIsInitialized) {
    Log::e("Variable '" + mName + "' is used before it is initialized");
    exit(1);
  }
  
  return IRWriter::newLoadInst(context, mValue, "");
}

Value* LocalReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  mType->incremenetReferenceCount(context, newValue);
  
  IRWriter::newStoreInst(context, newValue, mValue);

  mIsInitialized = true;
  
  return mValue;
}

void LocalReferenceVariable::setToNull(IRGenerationContext& context) {
  PointerType* llvmType = mType->getLLVMType(context.getLLVMContext());
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), mValue);

  mIsInitialized = true;
}

void LocalReferenceVariable::free(IRGenerationContext& context) const {
}

bool LocalReferenceVariable::existsInOuterScope() const {
  return false;
}
