//
//  IntConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IntConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IVariable* IntConstant::getVariable(IRGenerationContext& context) const {
  return NULL;
}

llvm::Constant* IntConstant::generateIR(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), mValue, true);
}

const IType* IntConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::INT_TYPE;
}

void IntConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an int constant, it is not a heap pointer");
  exit(1);
}

void IntConstant::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to non owner type int constant expression");
  exit(1);
}

bool IntConstant::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

bool IntConstant::isConstant() const {
  return true;
}

void IntConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue;
}
