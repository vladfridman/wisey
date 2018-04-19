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
using namespace std;
using namespace wisey;

const EmptyExpression EmptyExpression::EMPTY_EXPRESSION = EmptyExpression();

EmptyExpression::EmptyExpression() { }

EmptyExpression::~EmptyExpression() { }

int EmptyExpression::getLine() const {
  return 0;
}

Value* EmptyExpression::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  return NULL;
}

const IType* EmptyExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::VOID_TYPE;
}

bool EmptyExpression::isConstant() const {
  return false;
}

bool EmptyExpression::isAssignable() const {
  return false;
}

void EmptyExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
}

