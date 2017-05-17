//
//  DoubleType.cpp
//  Yazyk
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
using namespace yazyk;

string DoubleType::getName() const {
  return "double";
}

llvm::Type* DoubleType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getDoubleTy(llvmContext);
}

TypeKind DoubleType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool DoubleType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool DoubleType::canAutoCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* DoubleType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
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
  Cast::exitIncopatibleTypes(this, toType);
  return NULL;
}
