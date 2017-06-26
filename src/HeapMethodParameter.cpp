//
//  HeapMethodParameter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/HeapMethodParameter.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapMethodParameter::getName() const {
  return mName;
}

const IType* HeapMethodParameter::getType() const {
  return mType;
}

Value* HeapMethodParameter::getValue() const {
  return mValue;
}

Value* HeapMethodParameter::generateIdentifierIR(IRGenerationContext& context,
                                                 string llvmVariableName) const {
  return mValue;
}

Value* HeapMethodParameter::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void HeapMethodParameter::free(IRGenerationContext& context) const {
  TypeKind typeKind = mType->getTypeKind();
  if (typeKind != INTERFACE_OWNER_TYPE &&
      typeKind != MODEL_OWNER_TYPE &&
      typeKind != CONTROLLER_OWNER_TYPE) {
    return;
  }
  
  Value* thisPointer = mType->getTypeKind() == INTERFACE_OWNER_TYPE
    ? Interface::getOriginalObject(context, mValue)
    : mValue;
  
  IRWriter::createFree(context, thisPointer);
}

bool HeapMethodParameter::existsInOuterScope() const {
  return true;
}