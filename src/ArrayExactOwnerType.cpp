//
//  ArrayExactOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayExactOwnerType.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArrayExactOwnerType::ArrayExactOwnerType(const ArrayExactType* arrayExactType) :
mArrayExactType(arrayExactType) {
}

ArrayExactOwnerType::~ArrayExactOwnerType() {
}

const ArrayExactType* ArrayExactOwnerType::getArrayExactType() const {
  return mArrayExactType;
}

string ArrayExactOwnerType::getTypeName() const {
  return mArrayExactType->getTypeName() + "*";
}

llvm::PointerType* ArrayExactOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArrayExactType->getLLVMType(context);
}

TypeKind ArrayExactOwnerType::getTypeKind() const {
  return ARRAY_OWNER_TYPE;
}

bool ArrayExactOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (!IType::isArrayType(toType)) {
    return false;
  }
  
  const ArrayType* toArrayType = toType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) toType : ((const ArrayOwnerType*) toType)->getArrayType();
  
  return toArrayType->getElementType() == mArrayExactType->getElementType() &&
  toArrayType->getNumberOfDimensions() == mArrayExactType->getNumberOfDimensions();
}

bool ArrayExactOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArrayExactOwnerType::castTo(IRGenerationContext &context,
                                         llvm::Value* fromValue,
                                         const IType* toType,
                                         int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
}

void ArrayExactOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  // This should never be called
  assert(false);
}

bool ArrayExactOwnerType::isOwner() const {
  return true;
}

void ArrayExactOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayExactOwnerType::allocateVariable(IRGenerationContext& context, string name) const {
  assert(false);
}
