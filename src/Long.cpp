//
//  Long.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Long.hpp"

using namespace llvm;
using namespace yazyk;

Value* Long::generateIR(IRGenerationContext& context) {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}
