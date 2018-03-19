//
//  LLVMi16Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi16Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi16Type::LLVMi16Type() {
  mPointerType = new LLVMPointerType(this);
}

LLVMi16Type::~LLVMi16Type() {
  delete mPointerType;
}

string LLVMi16Type::getTypeName() const {
  return "::llvm::i16";
}

llvm::Type* LLVMi16Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt16Ty(context.getLLVMContext());
}

bool LLVMi16Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

bool LLVMi16Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

Value* LLVMi16Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this) {
    return fromValue;
  }
  assert(false);
  return NULL;
}

bool LLVMi16Type::isPrimitive() const {
  return false;
}

bool LLVMi16Type::isOwner() const {
  return false;
}

bool LLVMi16Type::isReference() const {
  return false;
}

bool LLVMi16Type::isArray() const {
  return false;
}

bool LLVMi16Type::isFunction() const {
  return false;
}

bool LLVMi16Type::isPackage() const {
  return false;
}

bool LLVMi16Type::isController() const {
  return false;
}

bool LLVMi16Type::isInterface() const {
  return false;
}

bool LLVMi16Type::isModel() const {
  return false;
}

bool LLVMi16Type::isNode() const {
  return false;
}

bool LLVMi16Type::isThread() const {
  return false;
}

bool LLVMi16Type::isNative() const {
  return true;
}

void LLVMi16Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi16Type::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMi16Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMi16Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMi16Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMi16Type::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMi16Type::getPointerType() const {
  return mPointerType;
}

const IType* LLVMi16Type::getDereferenceType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMi16Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}
