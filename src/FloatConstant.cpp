//
//  FloatConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FloatConstant.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* FloatConstant::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}

IType* FloatConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::FLOAT_TYPE;
}

void FloatConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a float constant, it is not a heap pointer");
  exit(1);
}
