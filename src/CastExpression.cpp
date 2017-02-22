//
//  CastExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/CastExpression.hpp"

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
