//
//  BooleanType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/BooleanType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string BooleanType::getName() const {
  return "boolean";
}

llvm::Type* BooleanType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt1Ty(llvmContext);
}

TypeKind BooleanType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool BooleanType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool BooleanType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::BOOLEAN_TYPE ||
    toType == PrimitiveTypes::CHAR_TYPE ||
    toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::FLOAT_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* BooleanType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::CHAR_TYPE ||
             toType == PrimitiveTypes::INT_TYPE ||
             toType == PrimitiveTypes::LONG_TYPE) {
    return AutoCast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return AutoCast::intToFloatCast(context, fromValue, toType);
  }
  AutoCast::exitIncopatibleTypes(this, toType);
  return NULL;
}
