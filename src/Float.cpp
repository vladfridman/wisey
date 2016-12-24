//
//  Float.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Float.hpp"

using namespace llvm;
using namespace yazyk;

Value* Float::generateIR(IRGenerationContext& context) {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}
