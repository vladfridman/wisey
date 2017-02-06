//
//  PrimitiveTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

Type* PrimitiveTypeSpecifier::getLLVMType(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  if (mType == PRIMITIVE_TYPE_BOOLEAN) {
    return (Type*) Type::getInt1Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_CHAR) {
    return (Type*) Type::getInt8Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_INT) {
    return (Type*) Type::getInt32Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_LONG) {
    return (Type*)  Type::getInt64Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_FLOAT) {
    return Type::getFloatTy(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_DOUBLE) {
    return Type::getDoubleTy(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_VOID) {
    return Type::getVoidTy(llvmContext);
  }
  
  Log::e("Unknown primitive type " + std::to_string(mType) + ". Replacing with void.");
  return Type::getVoidTy(llvmContext);
}

VariableStorageType PrimitiveTypeSpecifier::getStorageType() const {
  return STACK_VARIABLE;
}
