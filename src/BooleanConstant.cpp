//
//  BooleanConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/BooleanConstant.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* BooleanConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), mValue);
}

IType* BooleanConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}
