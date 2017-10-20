//
//  DoubleConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DoubleConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

IVariable* DoubleConstant::getVariable(IRGenerationContext& context) const {
  return NULL;
}

llvm::Constant* DoubleConstant::generateIR(IRGenerationContext& context) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}

const IType* DoubleConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::DOUBLE_TYPE;
}

void DoubleConstant::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of a double constant, it is not a heap pointer");
  exit(1);
}

void DoubleConstant::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to non owner type double constant expression");
  exit(1);
}

bool DoubleConstant::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

bool DoubleConstant::isConstant() const {
  return true;
}

void DoubleConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue << "d";
}
