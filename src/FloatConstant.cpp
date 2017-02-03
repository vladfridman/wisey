//
//  FloatConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/FloatConstant.hpp"

using namespace llvm;
using namespace yazyk;

Value* FloatConstant::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}
