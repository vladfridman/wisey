//
//  TypeIdentifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Type.h>

#include "yazyk/log.hpp"
#include "yazyk/TypeIdentifier.hpp"

using namespace llvm;
using namespace yazyk;

Type* TypeIdentifier::typeOf(LLVMContext &llvmContext, const TypeSpecifier& typeSpecifier) {
  PrimitiveType type = typeSpecifier.getType();
  if (type == PRIMITIVE_TYPE_INT) {
    return (Type*) Type::getInt32Ty(llvmContext);
  } else if (type == PRIMITIVE_TYPE_LONG) {
    return (Type*)  Type::getInt64Ty(llvmContext);
  } else if (type == PRIMITIVE_TYPE_FLOAT) {
    return Type::getFloatTy(llvmContext);
  } else if (type == PRIMITIVE_TYPE_DOUBLE) {
    return Type::getDoubleTy(llvmContext);
  } else if (type == PRIMITIVE_TYPE_VOID) {
    return Type::getVoidTy(llvmContext);
  }
  
  Log::e("Unknown type " + std::to_string(type) + ". Replacing with void.");
  return Type::getVoidTy(llvmContext);
}

