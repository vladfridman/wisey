//
//  TypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/TypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

Type* TypeSpecifier::getType(LLVMContext& llvmContext) const {
  if (mType == PRIMITIVE_TYPE_INT32) {
    return (Type*) Type::getInt32Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_INT64) {
    return (Type*)  Type::getInt64Ty(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_FLOAT32) {
    return Type::getFloatTy(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_FLOAT64) {
    return Type::getDoubleTy(llvmContext);
  } else if (mType == PRIMITIVE_TYPE_VOID) {
    return Type::getVoidTy(llvmContext);
  }
  
  Log::e("Unknown primitive type " + std::to_string(mType) + ". Replacing with void.");
  return Type::getVoidTy(llvmContext);
}
