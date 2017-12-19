//
//  ArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayType.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

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

const IType* ArrayType::getScalarType() const {
  return mBaseType->getTypeKind() == ARRAY_TYPE
  ? ((const ArrayType*) mBaseType)->getScalarType()
  : mBaseType;
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

void ArrayType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  assert(IType::isOwnerType(getScalarType()));
  const IType* baseType = mBaseType;
  unsigned long size = mSize;
  while (baseType->getTypeKind() == ARRAY_TYPE) {
    const ArrayType* arrayType = (const ArrayType*) baseType;
    baseType = arrayType->getBaseType();
    size = size * arrayType->getSize();
  }

  llvm::LLVMContext& llvmContext = context.getLLVMContext();

  llvm::Type* int64type = llvm::Type::getInt64Ty(llvmContext);
  llvm::Value* indexStore = IRWriter::newAllocaInst(context, int64type, "");
  llvm::Constant* int64zero = llvm::ConstantInt::get(int64type, 0);
  IRWriter::newStoreInst(context, int64zero, indexStore);
  
  llvm::Type* genericPointer = llvm::Type::getInt8Ty(llvmContext)->getPointerTo();
  llvm::Type* genericArray = llvm::ArrayType::get(genericPointer, 0)->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericArray);
  const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) getScalarType();
  llvm::Function* destructor = objectOwnerType->getDestructorFunction(context);

  DestroyOwnerArrayFunction::call(context, arrayBitcast, size, destructor);
}
