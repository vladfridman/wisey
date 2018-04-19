//
//  CharConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CharConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

CharConstant::CharConstant(char value, int line) : mValue(value), mLine(line) {
}

CharConstant::~CharConstant() {
}

int CharConstant::getLine() const {
  return mLine;
}

llvm::Constant* CharConstant::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  return ConstantInt::get(Type::getInt16Ty(context.getLLVMContext()), mValue);
}

const IType* CharConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::CHAR_TYPE;
}

bool CharConstant::isConstant() const {
  return true;
}

bool CharConstant::isAssignable() const {
  return false;
}

void CharConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "'" << mValue << "'";
}
