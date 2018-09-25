//
//  NullExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "NullExpression.hpp"
#include "NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NullExpression::NullExpression(int line) : mLine(line) { }

NullExpression::~NullExpression() { }

int NullExpression::getLine() const {
  return mLine;
}

llvm::Constant* NullExpression::generateIR(IRGenerationContext& context,
                                           const IType* assignToType) const {
  return ConstantExpr::getNullValue(getType(context)->getLLVMType(context));
}

const IType* NullExpression::getType(IRGenerationContext& context) const {
  return NullType::NULL_TYPE;
}

bool NullExpression::isConstant() const {
  return true;
}

bool NullExpression::isAssignable() const {
  return false;
}

void NullExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "null";
}
