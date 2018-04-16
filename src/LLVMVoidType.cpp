//
//  LLVMVoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMVoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMVoidType::LLVMVoidType() {
}

LLVMVoidType::~LLVMVoidType() {
}

string LLVMVoidType::getTypeName() const {
  return "::llvm::void";
}

llvm::Type* LLVMVoidType::getLLVMType(IRGenerationContext& context) const {
  return Type::getVoidTy(context.getLLVMContext());
}

bool LLVMVoidType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

bool LLVMVoidType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

Value* LLVMVoidType::castTo(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType,
                            int line) const {
  if (toType == this) {
    return fromValue;
  }
  assert(false);
  return NULL;
}

bool LLVMVoidType::isPrimitive() const {
  return false;
}

bool LLVMVoidType::isOwner() const {
  return false;
}

bool LLVMVoidType::isReference() const {
  return false;
}

bool LLVMVoidType::isArray() const {
  return false;
}

bool LLVMVoidType::isFunction() const {
  return false;
}

bool LLVMVoidType::isPackage() const {
  return false;
}

bool LLVMVoidType::isController() const {
  return false;
}

bool LLVMVoidType::isInterface() const {
  return false;
}

bool LLVMVoidType::isModel() const {
  return false;
}

bool LLVMVoidType::isNode() const {
  return false;
}

bool LLVMVoidType::isThread() const {
  return false;
}

bool LLVMVoidType::isNative() const {
  return true;
}

bool LLVMVoidType::isPointer() const {
  return false;
}

void LLVMVoidType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMVoidType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMVoidType::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMVoidType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMVoidType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const LLVMPointerType* LLVMVoidType::getPointerType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMVoidType::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}

Instruction* LLVMVoidType::inject(IRGenerationContext& context,
                                  const InjectionArgumentList injectionArgumentList,
                                  int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
