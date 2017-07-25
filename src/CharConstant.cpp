//
//  CharConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CharConstant.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* CharConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt16Ty(context.getLLVMContext()), mValue);
}

const IType* CharConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::CHAR_TYPE;
}

void CharConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a char constant, it is not a heap pointer");
  exit(1);
}

void CharConstant::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to non owner type char constant expression");
  exit(1);
}

bool CharConstant::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
