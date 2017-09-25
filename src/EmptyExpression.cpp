//
//  EmptyExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/EmptyExpression.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

const EmptyExpression EmptyExpression::EMPTY_EXPRESSION = EmptyExpression();

IVariable* EmptyExpression::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* EmptyExpression::generateIR(IRGenerationContext& context) const {
  return NULL;
}

const IType* EmptyExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::VOID_TYPE;
}

void EmptyExpression::releaseOwnership(IRGenerationContext& context) const {
  Log::e("Can not release ownership of an empty epxression, it is not a heap pointer");
  exit(1);
}

void EmptyExpression::addReferenceToOwner(IRGenerationContext& context,
                                          IVariable* reference) const {
  Log::e("Can not add a reference to non owner type empty expression");
  exit(1);
}

bool EmptyExpression::existsInOuterScope(IRGenerationContext& context) const {
  return false;
}

void EmptyExpression::printToStream(std::iostream& stream) const {
}

