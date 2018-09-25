//
//  CharConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "CharConstant.hpp"
#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

CharConstant::CharConstant(std::string value, int line) : mValue(value), mLine(line) {
}

CharConstant::~CharConstant() {
}

int CharConstant::getLine() const {
  return mLine;
}

llvm::Constant* CharConstant::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  if (!mValue.size()) {
    context.reportError(mLine, "Character in characted constant is not specified");
    throw 1;
  }
  char first = mValue.c_str()[0];
  if (mValue.size() == 1) {
    return ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), first);
  }
  if (first != '\\') {
    context.reportError(mLine, "String specified instead of a character constant");
    throw 1;
  }
  char second = mValue.c_str()[1];
  if (second == 'n') {
    return ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), '\n');
  }
  if (second == '0') {
    return ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), '\0');
  }

  context.reportError(mLine, "Unknown escape sequence");
  throw 1;
}

const IType* CharConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::CHAR;
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
