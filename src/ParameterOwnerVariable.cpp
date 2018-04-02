//
//  ParameterOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterOwnerVariable::ParameterOwnerVariable(string name,
                                               const IOwnerType* type,
                                               Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  if (!type->isNative()) {
    assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
  }
}

ParameterOwnerVariable::~ParameterOwnerVariable() {
}

string ParameterOwnerVariable::getName() const {
  return mName;
}

const IOwnerType* ParameterOwnerVariable::getType() const {
  return mType;
}

bool ParameterOwnerVariable::isField() const {
  return false;
}

bool ParameterOwnerVariable::isSystem() const {
  return false;
}

Value* ParameterOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* ParameterOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return mValueStore;
}

Value* ParameterOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
}

void ParameterOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded);
}

