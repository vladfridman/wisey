//
//  Double.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Double.hpp"

using namespace llvm;
using namespace yazyk;

Value* Double::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}
