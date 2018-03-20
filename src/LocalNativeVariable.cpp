//
//  LocalNativeVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalNativeVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalNativeVariable::LocalNativeVariable(string name, const IType* type, Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
  assert(valueStore->getType()->isPointerTy());
}

LocalNativeVariable::~LocalNativeVariable() {
}

string LocalNativeVariable::getName() const {
  return mName;
}

const IType* LocalNativeVariable::getType() const {
  return mType;
}

bool LocalNativeVariable::isField() const {
  return false;
}

bool LocalNativeVariable::isSystem() const {
  return false;
}

Value* LocalNativeVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalNativeVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return mValueStore;
}

Value* LocalNativeVariable::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression* assignToExpression,
                                                 vector<const IExpression*> arrayIndices,
                                                 int line) {
  assert(false);
}

