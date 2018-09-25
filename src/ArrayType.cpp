//
//  ArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

#include "ArrayOwnerType.hpp"
#include "ArrayType.hpp"
#include "Composer.hpp"
#include "FieldArrayReferenceVariable.hpp"
#include "IRWriter.hpp"
#include "ImmutableArrayType.hpp"
#include "LocalArrayReferenceVariable.hpp"
#include "Log.hpp"
#include "ParameterArrayReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

ArrayType::ArrayType(const IType* elementType, unsigned long numberOfDimensions) :
mElementType(elementType), mNumberOfDimensions(numberOfDimensions) {
  mArrayOwnerType = new ArrayOwnerType(this);
  mImmutableArrayType = new ImmutableArrayType(this);
}

ArrayType::~ArrayType() {
  delete mArrayOwnerType;
  delete mImmutableArrayType;
}

const unsigned int ArrayType::ARRAY_ELEMENTS_START_INDEX = 3u;

const ArrayOwnerType* ArrayType::getOwner() const {
  return mArrayOwnerType;
}

const ImmutableArrayType* ArrayType::getImmutable() const {
  return mImmutableArrayType;
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

bool ArrayType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->isPointer() && toType->isNative()) {
    return true;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }

  return false;
}

bool ArrayType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ArrayType::castTo(IRGenerationContext &context,
                               llvm::Value* fromValue,
                               const IType* toType,
                               int line) const {
  if (toType == this) {
    return fromValue;
  }
  if (toType->isPointer() && toType->isNative()) {
    llvm::Value* arrayStart = ArrayType::extractLLVMArray(context, fromValue);
    return IRWriter::newBitCastInst(context, arrayStart, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 llvm::ICmpInst::ICMP_NE,
                                 fromValue,
                                 llvm::ConstantPointerNull::get(getLLVMType(context)),
                                 "");
  }

  return NULL;
}

void ArrayType::incrementReferenceCount(IRGenerationContext& context,
                                        llvm::Value* arrayPointer) const {
  Composer::incrementArrayReferenceCount(context, arrayPointer);
}

void ArrayType::decrementReferenceCount(IRGenerationContext& context,
                                        llvm::Value* arrayPointer) const {
  Composer::decrementArrayReferenceCount(context, arrayPointer);
}

unsigned long ArrayType::getNumberOfDimensions() const {
  return mNumberOfDimensions;
}

llvm::Value* ArrayType::bitcastToGenericPointer(IRGenerationContext& context,
                                                llvm::Value* arrayPointer) const {
  llvm::Type* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();
  return IRWriter::newBitCastInst(context, arrayPointer, genericPointer);
}

bool ArrayType::isPrimitive() const {
  return false;
}

bool ArrayType::isOwner() const {
  return false;
}

bool ArrayType::isReference() const {
  return true;
}

bool ArrayType::isArray() const {
  return true;
}

bool ArrayType::isFunction() const {
  return false;
}

bool ArrayType::isPackage() const {
  return false;
}

bool ArrayType::isController() const {
  return false;
}

bool ArrayType::isInterface() const {
  return false;
}

bool ArrayType::isModel() const {
  return false;
}

bool ArrayType::isNode() const {
  return false;
}

bool ArrayType::isNative() const {
  return false;
}

bool ArrayType::isPointer() const {
  return false;
}

bool ArrayType::isImmutable() const {
  return false;
}

void ArrayType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ArrayType::createLocalVariable(IRGenerationContext &context,
                                    string name,
                                    int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalArrayReferenceVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

void ArrayType::createFieldVariable(IRGenerationContext& context,
                                    string name,
                                    const IConcreteObjectType* object,
                                    int line) const {
  IVariable* variable = new FieldArrayReferenceVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ArrayType::createParameterVariable(IRGenerationContext& context,
                                        string name,
                                        llvm::Value* value,
                                        int line) const {
  IVariable* variable = new ParameterArrayReferenceVariable(name, this,  value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(context, variable);
}

const ArrayType* ArrayType::getArrayType(IRGenerationContext& context, int line) const {
  return this;
}

llvm::Instruction* ArrayType::inject(IRGenerationContext& context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

llvm::Value* ArrayType::extractLLVMArray(IRGenerationContext& context, llvm::Value* arrayPointer) {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Value* index[2];
  index[0] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 0);
  index[1] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 3);
  
  return IRWriter::createGetElementPtrInst(context, arrayPointer, index);
}
