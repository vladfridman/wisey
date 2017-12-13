//
//  LocalArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

LocalArrayVariable::LocalArrayVariable(string name,
                                       const ArrayType* type,
                                       llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalArrayVariable::~LocalArrayVariable() {
}

string LocalArrayVariable::getName() const {
  return mName;
}

const ArrayType* LocalArrayVariable::getType() const {
  return mType;
}

llvm::Value* LocalArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValueStore;
}

llvm::Value* LocalArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                      IExpression* assignToExpression,
                                                      int line) {
  const IType* assignToType = assignToExpression->getType(context);
  Log::e("Trying to assign array " + getType()->getTypeName() +
         " to a non-compatible type " + assignToType->getTypeName());
  exit(1);
}

