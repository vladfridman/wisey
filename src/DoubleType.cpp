//
//  DoubleType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/DoubleType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string DoubleType::getTypeName() const {
  return "double";
}

llvm::Type* DoubleType::getLLVMType(IRGenerationContext& context) const {
  return Type::getDoubleTy(context.getLLVMContext());
}

TypeKind DoubleType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool DoubleType::canCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool DoubleType::canAutoCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* DoubleType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return Cast::truncFloatCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return fromValue;
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

string DoubleType::getFormat() const {
  return "%e";
}
