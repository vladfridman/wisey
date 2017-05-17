//
//  IntConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IntConstant.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* IntConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), mValue, true);
}

IType* IntConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::INT_TYPE;
}

void IntConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an int constant, it is not a heap pointer");
  exit(1);
}
