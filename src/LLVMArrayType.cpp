//
//  LLVMArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/LLVMArrayType.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace wisey;

LLVMArrayType::LLVMArrayType(const IType* elementType, list<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  assert(elementType->isNative());
  mPointerType = LLVMPointerType::create(this);
}

LLVMArrayType::~LLVMArrayType() {
  delete mPointerType;
}

list<unsigned long> LLVMArrayType::getDimensions() const {
  return mDimensions;
}

const IType* LLVMArrayType::getElementType() const {
  return mElementType;
}

string LLVMArrayType::getTypeName() const {
  string name = "::llvm::array(" + mElementType->getTypeName();
  for (long dimension : mDimensions) {
    name = name + ", " + to_string(dimension);
  }
  name = name + ")";

  return name;
}

llvm::ArrayType* LLVMArrayType::getLLVMType(IRGenerationContext& context) const {
  llvm::Type* type = mElementType->getLLVMType(context);
  list<unsigned long> dimensionsReversed = mDimensions;
  dimensionsReversed.reverse();
  for (unsigned long dimension : dimensionsReversed) {
    type = llvm::ArrayType::get(type, dimension);
  }
  
  return (llvm::ArrayType*) type;
}

bool LLVMArrayType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

bool LLVMArrayType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

llvm::Value* LLVMArrayType::castTo(IRGenerationContext &context,
                                   llvm::Value* fromValue,
                                   const IType* toType,
                                   int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

unsigned long LLVMArrayType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool LLVMArrayType::isPrimitive() const {
  return false;
}

bool LLVMArrayType::isOwner() const {
  return false;
}

bool LLVMArrayType::isReference() const {
  return false;
}

bool LLVMArrayType::isArray() const {
  return true;
}

bool LLVMArrayType::isFunction() const {
  return false;
}

bool LLVMArrayType::isPackage() const {
  return false;
}

bool LLVMArrayType::isController() const {
  return false;
}

bool LLVMArrayType::isInterface() const {
  return false;
}

bool LLVMArrayType::isModel() const {
  return false;
}

bool LLVMArrayType::isNode() const {
  return false;
}

bool LLVMArrayType::isThread() const {
  return false;
}

bool LLVMArrayType::isNative() const {
  return true;
}

bool LLVMArrayType::isPointer() const {
  return false;
}

void LLVMArrayType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMArrayType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMArrayType::createFieldVariable(IRGenerationContext& context,
                                        string name,
                                        const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMArrayType::createParameterVariable(IRGenerationContext& context,
                                            string name,
                                            llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMArrayType::getArrayType(IRGenerationContext& context) const {
  assert(false);
}

const LLVMPointerType* LLVMArrayType::getPointerType() const {
  return mPointerType;
}
