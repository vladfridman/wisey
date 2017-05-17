//
//  FloatType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/FloatType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

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

bool FloatType::canAutoCastTo(IType* toType) const {
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
    return Cast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::widenFloatCast(context, fromValue, toType);
  }
  Cast::exitIncopatibleTypes(this, toType);
  return NULL;
}
