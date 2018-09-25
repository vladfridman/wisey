//
//  ImmutableArrayType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

#include "Composer.hpp"
#include "FieldImmutableArrayReferenceVariable.hpp"
#include "IRWriter.hpp"
#include "ImmutableArrayType.hpp"
#include "LocalImmutableArrayReferenceVariable.hpp"
#include "ParameterImmutableArrayReferenceVariable.hpp"

using namespace std;
using namespace wisey;

ImmutableArrayType::ImmutableArrayType(const ArrayType* arrayType) : mArrayType(arrayType) {
  mImmutableArrayOwnerType = new ImmutableArrayOwnerType(this);
}

ImmutableArrayType::~ImmutableArrayType() {
  delete mImmutableArrayOwnerType;
}

const ImmutableArrayOwnerType* ImmutableArrayType::getOwner() const {
  return mImmutableArrayOwnerType;
}

const IType* ImmutableArrayType::getElementType() const {
  return mArrayType->getElementType();
}

string ImmutableArrayType::getTypeName() const {
  return "immutable " + mArrayType->getTypeName();
}

llvm::PointerType* ImmutableArrayType::getLLVMType(IRGenerationContext& context) const {
  return mArrayType->getLLVMType(context);
}

bool ImmutableArrayType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->isPointer() && toType->isNative()) {
    return true;
  }
  return false;
}

bool ImmutableArrayType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ImmutableArrayType::castTo(IRGenerationContext &context,
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

  return NULL;
}

void ImmutableArrayType::incrementReferenceCount(IRGenerationContext& context,
                                                 llvm::Value* arrayPointer) const {
  Composer::incrementImmutableArrayReferenceCount(context, arrayPointer);
}

void ImmutableArrayType::decrementReferenceCount(IRGenerationContext& context,
                                                 llvm::Value* arrayPointer) const {
  Composer::decrementImmutableArrayReferenceCount(context, arrayPointer);
}

unsigned long ImmutableArrayType::getNumberOfDimensions() const {
  return mArrayType->getNumberOfDimensions();
}

bool ImmutableArrayType::isPrimitive() const {
  return false;
}

bool ImmutableArrayType::isOwner() const {
  return false;
}

bool ImmutableArrayType::isReference() const {
  return true;
}

bool ImmutableArrayType::isArray() const {
  return true;
}

bool ImmutableArrayType::isFunction() const {
  return false;
}

bool ImmutableArrayType::isPackage() const {
  return false;
}

bool ImmutableArrayType::isController() const {
  return false;
}

bool ImmutableArrayType::isInterface() const {
  return false;
}

bool ImmutableArrayType::isModel() const {
  return false;
}

bool ImmutableArrayType::isNode() const {
  return false;
}

bool ImmutableArrayType::isNative() const {
  return false;
}

bool ImmutableArrayType::isPointer() const {
  return false;
}

bool ImmutableArrayType::isImmutable() const {
  return true;
}

void ImmutableArrayType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ImmutableArrayType::createLocalVariable(IRGenerationContext &context,
                                             string name,
                                             int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalImmutableArrayReferenceVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

void ImmutableArrayType::createFieldVariable(IRGenerationContext& context,
                                             string name,
                                             const IConcreteObjectType* object,
                                             int line) const {
  IVariable* variable = new FieldImmutableArrayReferenceVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ImmutableArrayType::createParameterVariable(IRGenerationContext& context,
                                                 string name,
                                                 llvm::Value* value,
                                                 int line) const {
  IVariable* variable = new ParameterImmutableArrayReferenceVariable(name, this,  value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(context, variable);
}

const ArrayType* ImmutableArrayType::getArrayType(IRGenerationContext& context, int line) const {
  return mArrayType;
}

llvm::Instruction* ImmutableArrayType::inject(IRGenerationContext& context,
                                              const InjectionArgumentList injectionArgumentList,
                                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
