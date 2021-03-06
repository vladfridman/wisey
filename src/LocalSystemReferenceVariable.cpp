//
//  LocalSystemReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "Composer.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalSystemReferenceVariable.hpp"
#include "Log.hpp"
#include "Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalSystemReferenceVariable::LocalSystemReferenceVariable(string name,
                                                           const IObjectType* type,
                                                           Value* valueStore,
                                                           int line) :
mName(name), mType(type), mValueStore(valueStore), mIsInitialized(false), mLine(line) {
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

bool LocalSystemReferenceVariable::isStatic() const {
  return true;
}

int LocalSystemReferenceVariable::getLine() const {
  return mLine;
}

Value* LocalSystemReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                          int line) const {
  if (!mIsInitialized) {
    context.reportError(line, "System variable '" + mName + "' is not initialized");
    throw 1;
  }
  
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalSystemReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                   int line) const {
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

