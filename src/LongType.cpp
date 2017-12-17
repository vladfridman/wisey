//
//  LongType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayElementType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LongType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LongType::LongType() : mArrayElementType(new ArrayElementType(this)) {
}

LongType::~LongType() {
  delete mArrayElementType;
}

string LongType::getTypeName() const {
  return "long";
}

llvm::Type* LongType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt64Ty(context.getLLVMContext());
}

TypeKind LongType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool LongType::canCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool LongType::canAutoCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::LONG_TYPE;
}

Value* LongType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::LONG_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

string LongType::getFormat() const {
  return "%d";
}

const ArrayElementType* LongType::getArrayElementType() const {
  return mArrayElementType;
}

