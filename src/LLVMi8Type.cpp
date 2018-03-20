//
//  LLVMi8Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi8Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi8Type::LLVMi8Type() {
  mPointerType = new LLVMPointerType(this);
}

LLVMi8Type::~LLVMi8Type() {
  delete mPointerType;
}

string LLVMi8Type::getTypeName() const {
  return "::llvm::i8";
}

llvm::Type* LLVMi8Type::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext());
}

bool LLVMi8Type::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi8Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi8Type::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
}

bool LLVMi8Type::isPrimitive() const {
  return false;
}

bool LLVMi8Type::isOwner() const {
  return false;
}

bool LLVMi8Type::isReference() const {
  return false;
}

bool LLVMi8Type::isArray() const {
  return false;
}

bool LLVMi8Type::isFunction() const {
  return false;
}

bool LLVMi8Type::isPackage() const {
  return false;
}

bool LLVMi8Type::isController() const {
  return false;
}

bool LLVMi8Type::isInterface() const {
  return false;
}

bool LLVMi8Type::isModel() const {
  return false;
}

bool LLVMi8Type::isNode() const {
  return false;
}

bool LLVMi8Type::isThread() const {
  return false;
}

bool LLVMi8Type::isNative() const {
  return true;
}

void LLVMi8Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi8Type::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMi8Type::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMi8Type::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMi8Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LLVMi8Type::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMi8Type::getPointerType() const {
  return mPointerType;
}

const ILLVMType* LLVMi8Type::getDereferenceType() const {
  assert(false);
}

const ILLVMTypeSpecifier* LLVMi8Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}
