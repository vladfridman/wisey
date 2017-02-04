//
//  CharConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/CharConstant.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* CharConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), mValue);
}

IType* CharConstant::getType() const {
  return PrimitiveTypes::CHARTYPE;
}
