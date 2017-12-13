//
//  ArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayType.hpp"

using namespace std;
using namespace wisey;

ArrayType::ArrayType(const IType* baseType, unsigned long size) : mBaseType(baseType), mSize(size) {
}

ArrayType::~ArrayType() {
}

const IType* ArrayType::getBaseType() const {
  return mBaseType;
}

unsigned long ArrayType::getSize() const {
  return mSize;
}

string ArrayType::getTypeName() const {
  return mBaseType->getTypeName() + "[" + to_string(mSize) + "]";
}

llvm::Type* ArrayType::getLLVMType(IRGenerationContext& context) const {
  return llvm::ArrayType::get(mBaseType->getLLVMType(context), mSize);
}

TypeKind ArrayType::getTypeKind() const {
  return ARRAY_TYPE;
}

bool ArrayType::canCastTo(const IType *toType) const {
  return toType == this;
}

bool ArrayType::canAutoCastTo(const IType *toType) const {
  return toType == this;
}

llvm::Value* ArrayType::castTo(IRGenerationContext &context,
                               llvm::Value* fromValue,
                               const IType* toType,
                               int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}
