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

IVariable* NullExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* NullExpression::generateIR(IRGenerationContext& context) const {
  return ConstantExpr::getNullValue(getType(context)->getLLVMType(context.getLLVMContext()));
}

const IType* NullExpression::getType(IRGenerationContext& context) const {
  return NullType::NULL_TYPE;
}

void NullExpression::releaseOwnership(IRGenerationContext& context) const {
}

void NullExpression::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  Log::e("Can not add a reference to a null expression");
  exit(1);
}

bool NullExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

void NullExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "null";
}
