//
//  ArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/DecrementReferencesInArrayFunction.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArrayType::ArrayType(const IType* elementType, vector<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  mArrayOwnerType = new ArrayOwnerType(this);
}

ArrayType::~ArrayType() {
  delete mArrayOwnerType;
}

const unsigned int ArrayType::ARRAY_ELEMENTS_START_INDEX = 2u;

const ArrayOwnerType* ArrayType::getOwner() const {
  return mArrayOwnerType;
}

vector<unsigned long> ArrayType::getDimensions() const {
  return mDimensions;
}

const IType* ArrayType::getElementType() const {
  return mElementType;
}

string ArrayType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long dimension : mDimensions) {
    name = name + "[" + to_string(dimension) + "]";
  }
  return name;
}

llvm::PointerType* ArrayType::getLLVMType(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Type* type = mElementType->getLLVMType(context);
  list<unsigned long> dimensionsReversed;
  for (unsigned long dimension : mDimensions) {
    dimensionsReversed.push_front(dimension);
  }
  for (unsigned long dimension : dimensionsReversed) {
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, dimension);
    vector<llvm::Type*> types;
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(arrayType);
    type = llvm::StructType::get(llvmContext, types);
  }

  return type->getPointerTo();
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

void ArrayType::incrementReferenceCount(IRGenerationContext& context,
                                        llvm::Value* arrayPointer) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, arrayPointer, 1);
}

void ArrayType::decrementReferenceCount(IRGenerationContext& context,
                                        llvm::Value* arrayPointer) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, arrayPointer, -1);
}

unsigned long ArrayType::getLinearSize() const {
  unsigned long size = 1;
  for (unsigned long dimension : mDimensions) {
    size *= dimension;
  }
  return size;
}

unsigned long ArrayType::getDimentionsSize() const {
  return mDimensions.size();
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

bool ArrayType::isOwner() const {
  return false;
}

