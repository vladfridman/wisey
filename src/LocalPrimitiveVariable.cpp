//
//  LocalPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string LocalPrimitiveVariable::getName() const {
  return mName;
}

const IType* LocalPrimitiveVariable::getType() const {
  return mType;
}

Value* LocalPrimitiveVariable::getValue() const {
  return mValue;
}

Value* LocalPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  return IRWriter::newLoadInst(context, mValue, llvmVariableName);
}

Value* LocalPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  return IRWriter::newStoreInst(context, castAssignToValue, mValue);
}

void LocalPrimitiveVariable::setToNull(IRGenerationContext& context) {
  Log::e("Stack Variables should not be set to null");
  exit(1);
}

void LocalPrimitiveVariable::free(IRGenerationContext& context) const {
}

bool LocalPrimitiveVariable::existsInOuterScope() const {
  return false;
}