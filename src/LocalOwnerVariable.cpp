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
                                       Value* valueStore,
                                       int line) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false), mLine(line) {
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

int LocalOwnerVariable::getLine() const {
  return mLine;
}

Value* LocalOwnerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  if (!mIsInitialized) {
    context.reportError(line, "Variable '" + mName + "' is used before it is initialized");
    throw 1;
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                         int line) const {
  return mValueStore;
}

Value* LocalOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                vector<const IExpression*> arrayIndices,
                                                int line) {
  Composer::setLineNumber(context, line);
  
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  free(context, line);

  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  mIsInitialized = true;
  
  return newValue;
}

void LocalOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  llvm::PointerType* type = getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
  mIsInitialized = true;
}

void LocalOwnerVariable::free(IRGenerationContext& context, int line) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded, line);
}

