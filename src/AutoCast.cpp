//
//  AutoCast.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* AutoCast::maybeCast(IRGenerationContext& context,
                           const IType* fromType,
                           Value* fromValue,
                           const IType* toType) {
  if (fromType == toType) {
    return fromValue;
  }
  if (!fromType->canCastTo(toType)) {
    Cast::exitIncompatibleTypes(fromType, toType);
  }
  if (!fromType->canAutoCastTo(toType)) {
    fromType->canAutoCastTo(toType);
    Log::e("Incompatible types: need explicit cast from type '" + fromType->getName() +
           "' to '" + toType->getName() + "'");
    exit(1);
  }
  
  return fromType->castTo(context, fromValue, toType);
}
