//
//  CastExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/AutoCast.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* CastExpression::generateIR(IRGenerationContext& context) const {
  IType* fromType = mExpression.getType(context);
  Value* fromValue = mExpression.generateIR(context);
  IType* toType = mTypeSpecifier.getType(context);
  
  return fromType->castTo(context, fromValue, toType);
}

IType* CastExpression::getType(IRGenerationContext& context) const {
  return mTypeSpecifier.getType(context);
}

void CastExpression::releaseOwnership(IRGenerationContext& context) const {
  IType* toType = mTypeSpecifier.getType(context);
  if (toType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Can not release ownership of a cast to primitive type, it is not a heap pointer");
    exit(1);
  }
  
  mExpression.releaseOwnership(context);
}
