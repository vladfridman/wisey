//
//  ByteConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ByteConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ByteConstant::ByteConstant(int value, int line) : mValue(value), mLine(line) {
}

ByteConstant::~ByteConstant() {
}

int ByteConstant::getLine() const {
  return mLine;
}

llvm::Constant* ByteConstant::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  return ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), mValue, true);
}

const IType* ByteConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BYTE;
}

bool ByteConstant::isConstant() const {
  return true;
}

bool ByteConstant::isAssignable() const {
  return false;
}

void ByteConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue;
}
