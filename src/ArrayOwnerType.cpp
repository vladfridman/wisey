//
//  ArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerType::ArrayOwnerType(const ArrayType* arrayType) : mArrayType(arrayType) {
}

ArrayOwnerType::~ArrayOwnerType() {
}

const ArrayType* ArrayOwnerType::getArrayType() const {
  return mArrayType;
}

string ArrayOwnerType::getTypeName() const {
  return mArrayType->getTypeName() + "*";
}

llvm::PointerType* ArrayOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mArrayType->getLLVMType(context);
}

TypeKind ArrayOwnerType::getTypeKind() const {
  return ARRAY_OWNER_TYPE;
}

bool ArrayOwnerType::canCastTo(const IType* toType) const {
  if (toType == this || toType == mArrayType) {
    return true;
  }
  
  return false;
}

bool ArrayOwnerType::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

llvm::Value* ArrayOwnerType::castTo(IRGenerationContext &context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this || toType == mArrayType) {
    return fromValue;
  }
  
  return NULL;
}

const IType* ArrayOwnerType::getBaseType() const {
  return mArrayType->getBaseType();
}

unsigned long ArrayOwnerType::getSize() const {
  return mArrayType->getSize();
}

const IType* ArrayOwnerType::getScalarType() const {
  return mArrayType->getScalarType();
}

void ArrayOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  const IType* elementType = getScalarType();
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
  
  if (IType::isOwnerType(elementType)) {
    const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) elementType;
    llvm::Value* destructor = objectOwnerType->getDestructorFunction(context);
    DestroyOwnerArrayFunction::call(context, arrayBitcast, destructor);
  } else if (IType::isReferenceType(elementType)) {
    DestroyReferenceArrayFunction::call(context, arrayBitcast);
  } else {
    assert(IType::isPrimitveType(elementType));
    DestroyPrimitiveArrayFunction::call(context, arrayBitcast);
  }
}

