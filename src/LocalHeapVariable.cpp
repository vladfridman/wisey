//
//  LocalHeapVariable.cpp
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
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string LocalHeapVariable::getName() const {
  return mName;
}

IType* LocalHeapVariable::getType() const {
  return mType;
}

Value* LocalHeapVariable::getValue() const {
  return mValue;
}

Value* LocalHeapVariable::generateIdentifierIR(IRGenerationContext& context,
                                                std::string llvmVariableName) const {
  if (mValue == NULL) {
    Log::e("Variable '" + mName + "' is used before it has been initialized.");
    exit(1);
  }
  return mValue;
}

Value* LocalHeapVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression& assignToExpression) {
  Value* assignToValue = assignToExpression.generateIR(context);
  IType* assignToType = assignToExpression.getType(context);
  mValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  return mValue;
}

void LocalHeapVariable::free(IRGenerationContext& context) const {
  if (mValue == NULL) {
    return;
  }
  
  if (mType->getTypeKind() == MODEL_TYPE) {
    IRWriter::createFree(context, mValue);
    return;
  }

  Value* thisPointer = Interface::getOriginalObject(context, mValue);
  IRWriter::createFree(context, thisPointer);
}
