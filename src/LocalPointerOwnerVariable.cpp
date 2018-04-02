//
//  LocalPointerOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPointerOwnerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalPointerOwnerVariable::LocalPointerOwnerVariable(string name,
                                                     const LLVMPointerOwnerType* type,
                                                     Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
  assert(valueStore->getType()->isPointerTy());
}

LocalPointerOwnerVariable::~LocalPointerOwnerVariable() {
}

string LocalPointerOwnerVariable::getName() const {
  return mName;
}

const LLVMPointerOwnerType* LocalPointerOwnerVariable::getType() const {
  return mType;
}

bool LocalPointerOwnerVariable::isField() const {
  return false;
}

bool LocalPointerOwnerVariable::isSystem() const {
  return false;
}

Value* LocalPointerOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalPointerOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                context) const {
  return mValueStore;
}

Value* LocalPointerOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                       IExpression* assignToExpression,
                                                       vector<const IExpression*> arrayIndices,
                                                       int line) {
  Composer::pushCallStack(context, line);
  
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  free(context);
  
  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  Composer::popCallStack(context);
  
  return newValue;
}

void LocalPointerOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
}

void LocalPointerOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded);
}
