//
//  Float32.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Float32.hpp"

using namespace llvm;
using namespace yazyk;

Value* Float32::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}
