//
//  DoubleConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/DoubleConstant.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* DoubleConstant::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}

IType* DoubleConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::DOUBLE_TYPE;
}

void DoubleConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a double constant, it is not a heap pointer");
  exit(1);
}
