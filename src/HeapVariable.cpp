//
//  HeapVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapVariable::getName() const {
  return mName;
}

const IType* HeapVariable::getType() const {
  return mType;
}

Value* HeapVariable::getValue() const {
  return mValue;
}

Value* HeapVariable::generateIdentifierIR(IRGenerationContext& context,
                                          string llvmVariableName) const {
  if (mValue == NULL) {
    Log::e("Variable '" + mName + "' is used before it has been initialized.");
    exit(1);
  }
  return mValue;
}

Value* HeapVariable::generateAssignmentIR(IRGenerationContext& context,
                                          IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  mValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  return mValue;
}

void HeapVariable::free(IRGenerationContext& context) const {
  if (mValue == NULL) {
    return;
  }
  
  if (!IType::isOwnerType(mType)) {
    return;
  }
  
  Value* thisPointer = mType->getTypeKind() == INTERFACE_OWNER_TYPE
    ? Interface::getOriginalObject(context, mValue)
    : mValue;
  
  IRWriter::createFree(context, thisPointer);
}

bool HeapVariable::existsInOuterScope() const {
  return false;
}
