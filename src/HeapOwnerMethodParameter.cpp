//
//  HeapOwnerMethodParameter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/HeapOwnerMethodParameter.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapOwnerMethodParameter::getName() const {
  return mName;
}

const IType* HeapOwnerMethodParameter::getType() const {
  return mType;
}

Value* HeapOwnerMethodParameter::getValue() const {
  return mValue;
}

Value* HeapOwnerMethodParameter::generateIdentifierIR(IRGenerationContext& context,
                                                      string llvmVariableName) const {
  return mValue;
}

Value* HeapOwnerMethodParameter::generateAssignmentIR(IRGenerationContext& context,
                                                      IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void HeapOwnerMethodParameter::setToNull(IRGenerationContext& context) const {
  PointerType* type = (PointerType*) getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValue);
}

void HeapOwnerMethodParameter::free(IRGenerationContext& context) const {
  mType->free(context, mValue);
}

bool HeapOwnerMethodParameter::existsInOuterScope() const {
  return true;
}
