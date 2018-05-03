//
//  LocalPointerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPointerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalPointerVariable::LocalPointerVariable(string name,
                                           const LLVMPointerType* type,
                                           Value* valueStore,
                                           int line) :
mName(name), mType(type), mValueStore(valueStore), mLine(line) {
  assert(valueStore->getType()->isPointerTy());
}

LocalPointerVariable::~LocalPointerVariable() {
}

string LocalPointerVariable::getName() const {
  return mName;
}

const LLVMPointerType* LocalPointerVariable::getType() const {
  return mType;
}

bool LocalPointerVariable::isField() const {
  return false;
}

bool LocalPointerVariable::isSystem() const {
  return false;
}

int LocalPointerVariable::getLine() const {
  return mLine;
}

Value* LocalPointerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalPointerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                           int line) const {
  return mValueStore;
}

Value* LocalPointerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                  IExpression* assignToExpression,
                                                  vector<const IExpression*> arrayIndices,
                                                  int line) {
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  
  IRWriter::newStoreInst(context, newValue, mValueStore);
  
  return newValue;
}

