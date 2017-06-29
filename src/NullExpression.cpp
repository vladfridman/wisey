//
//  NullExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* NullExpression::generateIR(IRGenerationContext& context) const {
  return ConstantExpr::getNullValue(getType(context)->getLLVMType(context.getLLVMContext()));
}

const IType* NullExpression::getType(IRGenerationContext& context) const {
  return NullType::NULL_TYPE;
}

void NullExpression::releaseOwnership(IRGenerationContext& context) const {
}

bool NullExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}
