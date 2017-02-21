//
//  CharType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Value.h>

#include "yazyk/AutoCast.hpp"
#include "yazyk/CharType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string CharType::getName() const {
  return "char";
}

llvm::Type* CharType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt16Ty(llvmContext);
}

TypeKind CharType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool CharType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool CharType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }

  return toType == PrimitiveTypes::CHAR_TYPE ||
    toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::FLOAT_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* CharType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return AutoCast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::CHAR_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::INT_TYPE || toType == PrimitiveTypes::LONG_TYPE) {
    return AutoCast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return AutoCast::intToFloatCast(context, fromValue, toType);
  }
  AutoCast::exitIncopatibleTypes(this, toType);
  return NULL;
}
