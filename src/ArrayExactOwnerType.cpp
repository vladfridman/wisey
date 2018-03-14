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
mArrayExactType(arrayExactType),
mPointerType(new PointerType(this)) {
}

ArrayExactOwnerType::~ArrayExactOwnerType() {
  delete mPointerType;
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

bool ArrayExactOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (!toType->isArray()) {
    return false;
  }
  
  const ArrayType* toArrayType = toType->getArrayType(context);
  
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

bool ArrayExactOwnerType::isPrimitive() const {
  return false;
}

bool ArrayExactOwnerType::isOwner() const {
  return true;
}

bool ArrayExactOwnerType::isReference() const {
  return false;
}

bool ArrayExactOwnerType::isArray() const {
  return true;
}

bool ArrayExactOwnerType::isFunction() const {
  return false;
}

bool ArrayExactOwnerType::isPackage() const {
  return false;
}

bool ArrayExactOwnerType::isController() const {
  return false;
}

bool ArrayExactOwnerType::isInterface() const {
  return false;
}

bool ArrayExactOwnerType::isModel() const {
  return false;
}

bool ArrayExactOwnerType::isNode() const {
  return false;
}

bool ArrayExactOwnerType::isThread() const {
  return false;
}

bool ArrayExactOwnerType::isNative() const {
  return false;
}

void ArrayExactOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayExactOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void ArrayExactOwnerType::createFieldVariable(IRGenerationContext& context,
                                              string name,
                                              const IConcreteObjectType* object) const {
  assert(false);
}

void ArrayExactOwnerType::createParameterVariable(IRGenerationContext& context,
                                                  string name,
                                                  llvm::Value* value) const {
  assert(false);
}

const ArrayType* ArrayExactOwnerType::getArrayType(IRGenerationContext& context) const {
  return mArrayExactType->getArrayType(context);
}

const IObjectType* ArrayExactOwnerType::getObjectType() const {
  return NULL;
}

const wisey::PointerType* ArrayExactOwnerType::getPointerType() const {
  return mPointerType;
}
