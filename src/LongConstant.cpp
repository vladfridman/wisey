//
//  LongConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/LongConstant.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* LongConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}

const IType* LongConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::LONG_TYPE;
}

void LongConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a long constant, it is not a heap pointer");
  exit(1);
}
