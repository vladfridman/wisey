//
//  ArrayExactType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayExactOwnerType.hpp"
#include "wisey/ArrayExactType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

ArrayExactType::ArrayExactType(const IType* elementType, list<unsigned long> dimensions) :
mElementType(elementType), mDimensions(dimensions) {
  assert(dimensions.size());
  mArrayExactOwnerType = new ArrayExactOwnerType(this);
}

ArrayExactType::~ArrayExactType() {
  delete mArrayExactOwnerType;
}

const ArrayExactOwnerType* ArrayExactType::getOwner() const {
  return mArrayExactOwnerType;
}

list<unsigned long> ArrayExactType::getDimensions() const {
  return mDimensions;
}

const IType* ArrayExactType::getElementType() const {
  return mElementType;
}

string ArrayExactType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long dimension : mDimensions) {
    name = name + "[" + to_string(dimension) + "]";
  }
  return name;
}

llvm::PointerType* ArrayExactType::getLLVMType(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Type* type = mElementType->getLLVMType(context);
  list<unsigned long> dimensionsReversed = mDimensions;
  dimensionsReversed.reverse();
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

bool ArrayExactType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

bool ArrayExactType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

llvm::Value* ArrayExactType::castTo(IRGenerationContext &context,
                                    llvm::Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

unsigned long ArrayExactType::getNumberOfDimensions() const {
  return mDimensions.size();
}

bool ArrayExactType::isPrimitive() const {
  return false;
}

bool ArrayExactType::isOwner() const {
  return false;
}

bool ArrayExactType::isReference() const {
  return true;
}

bool ArrayExactType::isArray() const {
  return true;
}

bool ArrayExactType::isFunction() const {
  return false;
}

bool ArrayExactType::isPackage() const {
  return false;
}

bool ArrayExactType::isController() const {
  return false;
}

bool ArrayExactType::isInterface() const {
  return false;
}

bool ArrayExactType::isModel() const {
  return false;
}

bool ArrayExactType::isNode() const {
  return false;
}

bool ArrayExactType::isNative() const {
  return false;
}

bool ArrayExactType::isPointer() const {
  return false;
}

bool ArrayExactType::isImmutable() const {
  return false;
}

void ArrayExactType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayExactType::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  assert(false);
}

void ArrayExactType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  assert(false);
}

void ArrayExactType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             llvm::Value* value,
                                             int line) const {
  assert(false);
}

const wisey::ArrayType* ArrayExactType::getArrayType(IRGenerationContext& context, int line) const {
  return context.getArrayType(mElementType, getNumberOfDimensions());
}

llvm::Instruction* ArrayExactType::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
