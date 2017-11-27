//
//  BooleanConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/BooleanConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IVariable* BooleanConstant::getVariable(IRGenerationContext& context) const {
  return NULL;
}

llvm::Constant* BooleanConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), mValue);
}

IType* BooleanConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

void BooleanConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a boolean constant, it is not a heap pointer");
  exit(1);
}

bool BooleanConstant::isConstant() const {
  return true;
}

void BooleanConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  if (mValue) {
    stream << "true";
  } else {
    stream << "false";
  }
}
