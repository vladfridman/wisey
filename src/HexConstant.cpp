//
//  HexConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/HexConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

HexConstant::HexConstant(unsigned long value, int line) : mValue(value), mLine(line) {
}

HexConstant::~HexConstant() {
}

int HexConstant::getLine() const {
  return mLine;
}

llvm::Constant* HexConstant::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), mValue, true);
}

const IType* HexConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::LONG;
}

bool HexConstant::isConstant() const {
  return true;
}

bool HexConstant::isAssignable() const {
  return false;
}

void HexConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "0x";
  stream << std::hex << mValue;
}
