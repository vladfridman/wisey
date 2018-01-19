//
//  ArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/DecrementReferencesInArrayFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArrayType::ArrayType(const IType* baseType, unsigned long size) : mBaseType(baseType), mSize(size) {
  mArrayOwnerType = new ArrayOwnerType(this);
}

ArrayType::~ArrayType() {
  delete mArrayOwnerType;
}

const ArrayOwnerType* ArrayType::getOwner() const {
  return mArrayOwnerType;
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

llvm::StructType* ArrayType::getLLVMType(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  const IType* baseType = mBaseType;
  
  list<unsigned long> dimentions;
  dimentions.push_front(mSize);
  
  while (IType::isArrayType(baseType)) {
    const IArrayType* baseArrayType = (const IArrayType*) mBaseType;
    dimentions.push_front(baseArrayType->getSize());
    baseType = baseArrayType->getBaseType();
  }
  
  llvm::Type* type = baseType->getLLVMType(context);
  vector<llvm::Type*> dimentionTypes;
  for (unsigned long size : dimentions) {
    type = llvm::ArrayType::get(type, size);
    dimentionTypes.push_back(llvm::Type::getInt64Ty(llvmContext));
  }
  llvm::StructType* subStruct = llvm::StructType::get(llvmContext, dimentionTypes);
  
  vector<llvm::Type*> types;
  types.push_back(llvm::Type::getInt64Ty(llvmContext));
  types.push_back(llvm::Type::getInt64Ty(llvmContext));
  types.push_back(subStruct);
  types.push_back(type);

  return llvm::StructType::get(llvmContext, types);
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
  llvm::Value* destructor = NULL;
  const IType* elementType = getScalarType();
  llvm::Value* arrayBitcast = bitcastToGenericPointer(context, arrayPointer);

  if (IType::isOwnerType(elementType)) {
    const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) elementType;
    destructor = objectOwnerType->getDestructorFunction(context);
    DestroyOwnerArrayFunction::call(context, arrayBitcast, getLinearSize(), destructor);
  } else if (IType::isReferenceType(elementType)) {
    llvm::FunctionType* destructorType = IConcreteObjectType::getDestructorFunctionType(context);
    destructor = llvm::ConstantPointerNull::get(destructorType->getPointerTo());
    DestroyReferenceArrayFunction::call(context, arrayBitcast, getLinearSize());
  } else {
    assert(IType::isPrimitveType(elementType));
    DestroyPrimitiveArrayFunction::call(context, arrayBitcast);
  }
}

void ArrayType::decrementReferenceCount(IRGenerationContext& context,
                                        llvm::Value* arrayPointer) const {
  assert(IType::isReferenceType(getScalarType()));
  const IObjectType* objectType = (const IObjectType*) getScalarType();
  llvm::Function* function = objectType->getReferenceAdjustmentFunction(context);
  llvm::Value* arrayBitcast = bitcastToGenericPointer(context, arrayPointer);

  DecrementReferencesInArrayFunction::call(context, arrayBitcast, getLinearSize(), function);
}

unsigned long ArrayType::getLinearSize() const {
  const IType* baseType = getBaseType();
  unsigned long size = getSize();
  while (baseType->getTypeKind() == ARRAY_TYPE) {
    const ArrayType* arrayType = (const ArrayType*) baseType;
    baseType = arrayType->getBaseType();
    size = size * arrayType->getSize();
  }
  
  return size;
}

unsigned long ArrayType::getDimentionsSize() const {
  const IType* baseType = getBaseType();
  unsigned long size = 1;
  while (baseType->getTypeKind() == ARRAY_TYPE) {
    const ArrayType* arrayType = (const ArrayType*) baseType;
    baseType = arrayType->getBaseType();
    size++;
  }
  
  return size;
}

llvm::PointerType* ArrayType::getGenericArrayType(IRGenerationContext& context) {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Type* genericPointer = llvm::Type::getInt8Ty(llvmContext)->getPointerTo();
  llvm::Type* genericArray = llvm::ArrayType::get(genericPointer, 0);
  
  return genericArray->getPointerTo();
}

llvm::Value* ArrayType::bitcastToGenericPointer(IRGenerationContext& context,
                                                llvm::Value* arrayPointer) const {
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  return IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
}
