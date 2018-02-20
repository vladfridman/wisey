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
#include "wisey/FieldArrayReferenceVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayReferenceVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

ArrayType::ArrayType(const IType* elementType, unsigned long numberOfDimensions) :
mElementType(elementType), mNumberOfDimensions(numberOfDimensions) {
  mArrayOwnerType = new ArrayOwnerType(this);
}

ArrayType::~ArrayType() {
  delete mArrayOwnerType;
}

const unsigned int ArrayType::ARRAY_ELEMENTS_START_INDEX = 3u;

const ArrayOwnerType* ArrayType::getOwner() const {
  return mArrayOwnerType;
}

const IType* ArrayType::getElementType() const {
  return mElementType;
}

string ArrayType::getTypeName() const {
  string name = mElementType->getTypeName();
  for (unsigned long i = 0u; i < mNumberOfDimensions; i++) {
    name = name + "[]";
  }
  return name;
}

llvm::PointerType* ArrayType::getLLVMType(IRGenerationContext& context) const {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Type* type = mElementType->getLLVMType(context);
  for (unsigned long i = 0; i < mNumberOfDimensions; i++) {
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, 0);
    vector<llvm::Type*> types;
    types.push_back(llvm::Type::getInt64Ty(llvmContext));
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

bool ArrayType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

bool ArrayType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
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

unsigned long ArrayType::getNumberOfDimensions() const {
  return mNumberOfDimensions;
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

bool ArrayType::isReference() const {
  return true;
}

void ArrayType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayType::allocateLocalVariable(IRGenerationContext &context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayReferenceVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

void ArrayType::createFieldVariable(IRGenerationContext& context,
                                    string name,
                                    const IConcreteObjectType* object) const {
  IVariable* variable = new FieldArrayReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void ArrayType::reportNonArrayType() {
  Log::e("Trying to perform array operation on a non-array type expression");
}

const ArrayType* ArrayType::getArrayType(IRGenerationContext& context) const {
  return this;
}
