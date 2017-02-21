//
//  FloatType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/FloatType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string FloatType::getName() const {
  return "float";
}

llvm::Type* FloatType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getFloatTy(llvmContext);
}

TypeKind FloatType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool FloatType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool FloatType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* FloatType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return AutoCast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return AutoCast::widenFloatCast(context, fromValue, toType);
  }
  AutoCast::exitIncopatibleTypes(this, toType);
  return NULL;
}
