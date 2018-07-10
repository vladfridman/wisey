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
                                               Value* valueStore,
                                               int line) :
mName(name), mType(type), mValueStore(valueStore), mLine(line) {
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

bool ParameterOwnerVariable::isStatic() const {
  return false;
}

int ParameterOwnerVariable::getLine() const {
  return mLine;
}

Value* ParameterOwnerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* ParameterOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                             int line) const {
  return mValueStore;
}

Value* ParameterOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  throw 1;
}

void ParameterOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
}

void ParameterOwnerVariable::free(IRGenerationContext& context, Value* exception, int line) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded, exception, line);
}

