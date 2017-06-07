//
//  IntType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/IntType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string IntType::getName() const {
  return "int";
}

llvm::Type* IntType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt32Ty(llvmContext);
}

TypeKind IntType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool IntType::canCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE;
}

bool IntType::canAutoCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* IntType::castTo(IRGenerationContext& context,
                       Value* fromValue,
                       const IType* toType) const {
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
