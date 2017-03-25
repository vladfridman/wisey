//
//  BooleanConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/BooleanConstant.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* BooleanConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), mValue);
}

IType* BooleanConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void BooleanConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a boolean constant, it is not a heap pointer");
  exit(1);
}
