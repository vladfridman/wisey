//
//  IntType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Cast.hpp"
#include "yazyk/IntType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string IntType::getName() const {
  return "int";
}

llvm::Type* IntType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt32Ty(llvmContext);
}

TypeKind IntType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool IntType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool IntType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* IntType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE || toType == PrimitiveTypes::CHAR_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::INT_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncopatibleTypes(this, toType);
  return NULL;
}
