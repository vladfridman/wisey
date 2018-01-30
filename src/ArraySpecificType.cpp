//
//  ArraySpecificType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ArraySpecificType.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArraySpecificType::ArraySpecificType(const IType* elementType, vector<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  mArraySpecificOwnerType = new ArraySpecificOwnerType(this);
}

ArraySpecificType::~ArraySpecificType() {
  delete mArraySpecificOwnerType;
}

const ArraySpecificOwnerType* ArraySpecificType::getOwner() const {
  return mArraySpecificOwnerType;
}

wisey::ArrayType* ArraySpecificType::getArrayType(IRGenerationContext& context) const {
  return context.getArrayType(mElementType, mDimensions.size());
}

vector<unsigned long> ArraySpecificType::getDimensions() const {
  return mDimensions;
}

const IType* ArraySpecificType::getElementType() const {
  return mElementType;
}

string ArraySpecificType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long dimension : mDimensions) {
    name = name + "[" + to_string(dimension) + "]";
  }
  return name;
}

llvm::PointerType* ArraySpecificType::getLLVMType(IRGenerationContext& context) const {
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
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
    types.push_back(arrayType);
    type = llvm::StructType::get(llvmContext, types);
  }
  
  return type->getPointerTo();
}

TypeKind ArraySpecificType::getTypeKind() const {
  return ARRAY_TYPE;
}

bool ArraySpecificType::canCastTo(const IType *toType) const {
  return toType == this;
}

bool ArraySpecificType::canAutoCastTo(const IType *toType) const {
  return toType == this;
}

llvm::Value* ArraySpecificType::castTo(IRGenerationContext &context,
                                       llvm::Value* fromValue,
                                       const IType* toType,
                                       int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

unsigned long ArraySpecificType::getLinearSize() const {
  unsigned long size = 1;
  for (unsigned long dimension : mDimensions) {
    size *= dimension;
  }
  return size;
}

unsigned long ArraySpecificType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool ArraySpecificType::isOwner() const {
  return false;
}
