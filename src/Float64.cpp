//
//  Float64.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Float64.hpp"

using namespace llvm;
using namespace yazyk;

Value* Float64::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}
