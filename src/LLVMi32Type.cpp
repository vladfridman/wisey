//
//  LLVMi32Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi32Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi32Type::LLVMi32Type() {
  mPointerType = new LLVMPointerType(this);
}

LLVMi32Type::~LLVMi32Type() {
  delete mPointerType;
}

string LLVMi32Type::getTypeName() const {
  return "::llvm::i32";
}

llvm::Type* LLVMi32Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt32Ty(context.getLLVMContext());
}

bool LLVMi32Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

bool LLVMi32Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

Value* LLVMi32Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this) {
    return fromValue;
  }
  assert(false);
  return NULL;
}

bool LLVMi32Type::isPrimitive() const {
  return false;
}

bool LLVMi32Type::isOwner() const {
  return false;
}

bool LLVMi32Type::isReference() const {
  return false;
}

bool LLVMi32Type::isArray() const {
  return false;
}

bool LLVMi32Type::isFunction() const {
  return false;
}

bool LLVMi32Type::isPackage() const {
  return false;
}

bool LLVMi32Type::isController() const {
  return false;
}

bool LLVMi32Type::isInterface() const {
  return false;
}

bool LLVMi32Type::isModel() const {
  return false;
}

bool LLVMi32Type::isNode() const {
  return false;
}

bool LLVMi32Type::isThread() const {
  return false;
}

bool LLVMi32Type::isNative() const {
  return true;
}

void LLVMi32Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi32Type::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMi32Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMi32Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMi32Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMi32Type::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMi32Type::getPointerType() const {
  return mPointerType;
}

const ILLVMType* LLVMi32Type::getDereferenceType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMi32Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}