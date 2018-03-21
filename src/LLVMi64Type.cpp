//
//  LLVMi64Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi64Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi64Type::LLVMi64Type() {
  mPointerType = new LLVMPointerType(this);
}

LLVMi64Type::~LLVMi64Type() {
  delete mPointerType;
}

string LLVMi64Type::getTypeName() const {
  return "::llvm::i64";
}

llvm::Type* LLVMi64Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt64Ty(context.getLLVMContext());
}

bool LLVMi64Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi64Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi64Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
}

bool LLVMi64Type::isPrimitive() const {
  return false;
}

bool LLVMi64Type::isOwner() const {
  return false;
}

bool LLVMi64Type::isReference() const {
  return false;
}

bool LLVMi64Type::isArray() const {
  return false;
}

bool LLVMi64Type::isFunction() const {
  return false;
}

bool LLVMi64Type::isPackage() const {
  return false;
}

bool LLVMi64Type::isController() const {
  return false;
}

bool LLVMi64Type::isInterface() const {
  return false;
}

bool LLVMi64Type::isModel() const {
  return false;
}

bool LLVMi64Type::isNode() const {
  return false;
}

bool LLVMi64Type::isThread() const {
  return false;
}

bool LLVMi64Type::isNative() const {
  return true;
}

void LLVMi64Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi64Type::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMi64Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMi64Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMi64Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMi64Type::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMi64Type::getPointerType() const {
  return mPointerType;
}

const ILLVMType* LLVMi64Type::getDereferenceType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMi64Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}
