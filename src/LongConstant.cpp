//
//  LongConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IRGenerationContext.hpp"
#include "LongConstant.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LongConstant::LongConstant(long long value, int line) : mValue(value), mLine(line) {
}

LongConstant::~LongConstant() {
}

int LongConstant::getLine() const {
  return mLine;
}

llvm::Constant* LongConstant::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}

const IType* LongConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::LONG;
}

bool LongConstant::isConstant() const {
  return true;
}

bool LongConstant::isAssignable() const {
  return false;
}

void LongConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue << "l";
}
