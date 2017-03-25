//
//  LongConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/LongConstant.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* LongConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}

IType* LongConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::LONG_TYPE;
}

void LongConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a long constant, it is not a heap pointer");
  exit(1);
}
