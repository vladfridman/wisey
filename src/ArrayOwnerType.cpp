//
//  ArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerType::ArrayOwnerType(const ArrayType* arrayType) : mArrayType(arrayType) {
}

ArrayOwnerType::~ArrayOwnerType() {
}

const ArrayType* ArrayOwnerType::getArrayType(IRGenerationContext& context) const {
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

bool ArrayOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mArrayType) {
    return true;
  }
  
  return false;
}

bool ArrayOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
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

bool ArrayOwnerType::isReference() const {
  return false;
}

void ArrayOwnerType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayOwnerType::allocateLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}
