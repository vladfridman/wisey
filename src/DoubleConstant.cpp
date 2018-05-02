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
using namespace std;
using namespace wisey;

DoubleConstant::DoubleConstant(long double value, int line) : mValue(value), mLine(line) {
}

DoubleConstant::~DoubleConstant() {
}

int DoubleConstant::getLine() const {
  return mLine;
}

llvm::Constant* DoubleConstant::generateIR(IRGenerationContext& context,
                                           const IType* assignToType) const {
  return ConstantFP::get(Type::getDoubleTy(context.getLLVMContext()), mValue);
}

const IType* DoubleConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::DOUBLE;
}

bool DoubleConstant::isConstant() const {
  return true;
}

bool DoubleConstant::isAssignable() const {
  return false;
}

void DoubleConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue << "d";
}
