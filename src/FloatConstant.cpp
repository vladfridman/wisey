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
using namespace std;
using namespace wisey;

FloatConstant::FloatConstant(double value, int line) : mValue(value), mLine(line) {
}

FloatConstant::~FloatConstant() {
}

int FloatConstant::getLine() const {
  return mLine;
}

llvm::Constant* FloatConstant::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  return ConstantFP::get(Type::getFloatTy(context.getLLVMContext()), mValue);
}

const IType* FloatConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::FLOAT_TYPE;
}

bool FloatConstant::isConstant() const {
  return true;
}

bool FloatConstant::isAssignable() const {
  return false;
}

void FloatConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue;
}
