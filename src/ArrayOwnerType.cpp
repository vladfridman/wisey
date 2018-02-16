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

void ArrayOwnerType::free(IRGenerationContext& context, llvm::Value* arrayPointer) const {
  const IType* elementType = mArrayType->getElementType();
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  llvm::Value* arrayBitcast = IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
  
  if (IType::isOwnerType(elementType)) {
    const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) elementType;
    llvm::Value* destructor = objectOwnerType->getDestructorFunction(context);
    DestroyOwnerArrayFunction::call(context,
                                    arrayBitcast,
                                    mArrayType->getNumberOfDimensions(),
                                    destructor);
  } else if (IType::isReferenceType(elementType)) {
    DestroyReferenceArrayFunction::call(context, arrayBitcast, mArrayType->getNumberOfDimensions());
  } else {
    assert(IType::isPrimitveType(elementType));
    const IPrimitiveType* primitiveType = (const IPrimitiveType*) elementType;
    DestroyPrimitiveArrayFunction::call(context,
                                        arrayBitcast,
                                        mArrayType->getNumberOfDimensions(),
                                        primitiveType);
  }
}

bool ArrayOwnerType::isOwner() const {
  return true;
}

void ArrayOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}
