//
//  BooleanConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "BooleanConstant.hpp"
#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BooleanConstant::BooleanConstant(bool value, int line) : mValue(value), mLine(line) {
}

BooleanConstant::~BooleanConstant() {
}

int BooleanConstant::getLine() const {
  return mLine;
}

llvm::Constant* BooleanConstant::generateIR(IRGenerationContext& context,
                                            const IType* assignToType) const {
  return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), mValue);
}

IType* BooleanConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN;
}

bool BooleanConstant::isConstant() const {
  return true;
}

bool BooleanConstant::isAssignable() const {
  return false;
}

void BooleanConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  if (mValue) {
    stream << "true";
  } else {
    stream << "false";
  }
}
