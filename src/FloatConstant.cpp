//
//  FloatConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FloatConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IVariable* FloatConstant::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* FloatConstant::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}

const IType* FloatConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::FLOAT_TYPE;
}

void FloatConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a float constant, it is not a heap pointer");
  exit(1);
}

void FloatConstant::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to non owner type float constant expression");
  exit(1);
}

bool FloatConstant::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

bool FloatConstant::isConstant() const {
  return true;
}

void FloatConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue;
}
