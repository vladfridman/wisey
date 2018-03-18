//
//  LLVMi1Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi1Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi1Type::LLVMi1Type() {
  mPointerType = new PointerType(this);
}

LLVMi1Type::~LLVMi1Type() {
  delete mPointerType;
}

string LLVMi1Type::getTypeName() const {
  return "::llvm::i1";
}

llvm::Type* LLVMi1Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt1Ty(context.getLLVMContext());
}

bool LLVMi1Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

bool LLVMi1Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType != this;
}

Value* LLVMi1Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this) {
    return fromValue;
  }
  assert(false);
  return NULL;
}

bool LLVMi1Type::isPrimitive() const {
  return false;
}

bool LLVMi1Type::isOwner() const {
  return false;
}

bool LLVMi1Type::isReference() const {
  return false;
}

bool LLVMi1Type::isArray() const {
  return false;
}

bool LLVMi1Type::isFunction() const {
  return false;
}

bool LLVMi1Type::isPackage() const {
  return false;
}

bool LLVMi1Type::isController() const {
  return false;
}

bool LLVMi1Type::isInterface() const {
  return false;
}

bool LLVMi1Type::isModel() const {
  return false;
}

bool LLVMi1Type::isNode() const {
  return false;
}

bool LLVMi1Type::isThread() const {
  return false;
}

bool LLVMi1Type::isNative() const {
  return true;
}

void LLVMi1Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi1Type::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMi1Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMi1Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMi1Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMi1Type::getObjectType() const {
  return NULL;
}

const IType* LLVMi1Type::getPointerType() const {
  return mPointerType;
}

const IType* LLVMi1Type::getDereferenceType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMi1Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}
