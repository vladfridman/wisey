//
//  Int32.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Int32.hpp"

using namespace llvm;
using namespace yazyk;

Value* Int32::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), mValue, true);
}
