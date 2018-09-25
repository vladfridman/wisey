//
//  IntConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IntConstant.hpp"
#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IntConstant::IntConstant(long value, int line) : mValue(value), mLine(line) {
}

IntConstant::~IntConstant() {
}

int IntConstant::getLine() const {
  return mLine;
}

llvm::Constant* IntConstant::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), mValue, true);
}

const IType* IntConstant::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::INT;
}

bool IntConstant::isConstant() const {
  return true;
}

bool IntConstant::isAssignable() const {
  return false;
}

void IntConstant::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mValue;
}
