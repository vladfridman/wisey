//
//  LongType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Cast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LongType.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string LongType::getName() const {
  return "long";
}

llvm::Type* LongType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt64Ty(llvmContext);
}

TypeKind LongType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool LongType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool LongType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::LONG_TYPE;
}

Value* LongType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::LONG_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncopatibleTypes(this, toType);
  return NULL;
}
