//
//  ArraySpecificOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArraySpecificOwnerType::ArraySpecificOwnerType(const ArraySpecificType* arraySpecificType) :
mArraySpecificType(arraySpecificType) {
}

ArraySpecificOwnerType::~ArraySpecificOwnerType() {
}

const ArraySpecificType* ArraySpecificOwnerType::getArraySpecificType() const {
  return mArraySpecificType;
}

string ArraySpecificOwnerType::getTypeName() const {
  return mArraySpecificType->getTypeName() + "*";
}

llvm::PointerType* ArraySpecificOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArraySpecificType->getLLVMType(context);
}

TypeKind ArraySpecificOwnerType::getTypeKind() const {
  return ARRAY_OWNER_TYPE;
}

bool ArraySpecificOwnerType::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (!IType::isArrayType(toType)) {
    return false;
  }
  
  const ArrayType* toArrayType = toType->getTypeKind() == ARRAY_TYPE
  ? (const ArrayType*) toType : ((const ArrayOwnerType*) toType)->getArrayType();
  
  return toArrayType->getElementType() == mArraySpecificType->getElementType() &&
  toArrayType->getNumberOfDimensions() == mArraySpecificType->getNumberOfDimensions();
}

bool ArraySpecificOwnerType::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

llvm::Value* ArraySpecificOwnerType::castTo(IRGenerationContext &context,
                                            llvm::Value* fromValue,
                                            const IType* toType,
                                            int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
}

void ArraySpecificOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  // This should never be called
  assert(false);
}

bool ArraySpecificOwnerType::isOwner() const {
  return true;
}

