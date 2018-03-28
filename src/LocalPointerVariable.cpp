//
//  LocalPointerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPointerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalPointerVariable::LocalPointerVariable(string name, const IType* type, Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
  assert(valueStore->getType()->isPointerTy());
}

LocalPointerVariable::~LocalPointerVariable() {
}

string LocalPointerVariable::getName() const {
  return mName;
}

const IType* LocalPointerVariable::getType() const {
  return mType;
}

bool LocalPointerVariable::isField() const {
  return false;
}

bool LocalPointerVariable::isSystem() const {
  return false;
}

Value* LocalPointerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalPointerVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return mValueStore;
}

Value* LocalPointerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression* assignToExpression,
                                                 vector<const IExpression*> arrayIndices,
                                                 int line) {
  assert(false);
}

