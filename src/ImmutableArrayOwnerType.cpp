//
//  ImmutableImmutableArrayOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/LocalImmutableArrayOwnerVariable.hpp"

using namespace std;
using namespace wisey;

ImmutableArrayOwnerType::ImmutableArrayOwnerType(const ImmutableArrayType* immutableArrayType) :
mImmutableArrayType(immutableArrayType) {
}

ImmutableArrayOwnerType::~ImmutableArrayOwnerType() {
}

const ArrayType* ImmutableArrayOwnerType::getArrayType(IRGenerationContext& context) const {
  return mImmutableArrayType->getArrayType(context);
}

string ImmutableArrayOwnerType::getTypeName() const {
  return mImmutableArrayType->getTypeName() + "*";
}

llvm::PointerType* ImmutableArrayOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mImmutableArrayType->getLLVMType(context);
}

bool ImmutableArrayOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mImmutableArrayType) {
    return true;
  }
  
  return false;
}

bool ImmutableArrayOwnerType::canAutoCastTo(IRGenerationContext& context,
                                            const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* ImmutableArrayOwnerType::castTo(IRGenerationContext &context,
                                             llvm::Value* fromValue,
                                             const IType* toType,
                                             int line) const {
  if (toType == this || toType == mImmutableArrayType) {
    return fromValue;
  }
  
  return NULL;
}

void ImmutableArrayOwnerType::free(IRGenerationContext& context,
                                   llvm::Value* arrayPointer,
                                   int line) const {
  mImmutableArrayType->getArrayType(context)->getOwner()->free(context, arrayPointer, line);
}

bool ImmutableArrayOwnerType::isPrimitive() const {
  return false;
}

bool ImmutableArrayOwnerType::isOwner() const {
  return true;
}

bool ImmutableArrayOwnerType::isReference() const {
  return false;
}

bool ImmutableArrayOwnerType::isArray() const {
  return true;
}

bool ImmutableArrayOwnerType::isFunction() const {
  return false;
}

bool ImmutableArrayOwnerType::isPackage() const {
  return false;
}

bool ImmutableArrayOwnerType::isController() const {
  return false;
}

bool ImmutableArrayOwnerType::isInterface() const {
  return false;
}

bool ImmutableArrayOwnerType::isModel() const {
  return false;
}

bool ImmutableArrayOwnerType::isNode() const {
  return false;
}

bool ImmutableArrayOwnerType::isThread() const {
  return false;
}

bool ImmutableArrayOwnerType::isNative() const {
  return false;
}

bool ImmutableArrayOwnerType::isPointer() const {
  return false;
}

bool ImmutableArrayOwnerType::isImmutable() const {
  return true;
}

void ImmutableArrayOwnerType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void ImmutableArrayOwnerType::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloc);
  
  IVariable* variable = new LocalImmutableArrayOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

void ImmutableArrayOwnerType::createFieldVariable(IRGenerationContext& context,
                                                  string name,
                                                  const IConcreteObjectType* object) const {
  assert(false);
}

void ImmutableArrayOwnerType::createParameterVariable(IRGenerationContext& context,
                                                      string name,
                                                      llvm::Value* value) const {
  assert(false);
}

const IReferenceType* ImmutableArrayOwnerType::getReference() const {
  return NULL;
}

llvm::Instruction* ImmutableArrayOwnerType::inject(IRGenerationContext& context,
                                                   const InjectionArgumentList injectionArguments,
                                                   int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
