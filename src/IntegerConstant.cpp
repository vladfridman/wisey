//
//  IntegerConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/IntegerConstant.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* IntegerConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), mValue, true);
}

IType* IntegerConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::INT_TYPE;
}
