//
//  LLVMi32Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerType.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi32Type.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi32Type::LLVMi32Type() {
  mPointerType = LLVMPointerType::create(this);
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
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

bool LLVMi32Type::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this || toType->getLLVMType(context) == getLLVMType(context);
}

Value* LLVMi32Type::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == this || toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  }
  assert(false);
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

bool LLVMi32Type::isPointer() const {
  return false;
}

void LLVMi32Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi32Type::createLocalVariable(IRGenerationContext& context, string name) const {
  ILLVMType::createLocalLLVMVariable(context, this, name);
}

void LLVMi32Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  IVariable* variable = new FieldLLVMVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMi32Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LLVMi32Type::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const LLVMPointerType* LLVMi32Type::getPointerType() const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi32Type::newTypeSpecifier() const {
  return new LLVMPrimitiveTypeSpecifier(this);
}
