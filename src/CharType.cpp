//
//  CharType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Value.h>

#include "wisey/Cast.hpp"
#include "wisey/CharType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string CharType::getName() const {
  return "char";
}

llvm::Type* CharType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt16Ty(llvmContext);
}

TypeKind CharType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool CharType::canCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool CharType::canAutoCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }

  return toType == PrimitiveTypes::CHAR_TYPE ||
    toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::FLOAT_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* CharType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::CHAR_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::INT_TYPE || toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}
