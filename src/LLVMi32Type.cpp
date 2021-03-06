//
//  LLVMi32Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "FieldLLVMVariable.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPointerType.hpp"
#include "LLVMPrimitiveTypeSpecifier.hpp"
#include "LLVMi32Type.hpp"
#include "ParameterLLVMVariable.hpp"

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

bool LLVMi32Type::isNative() const {
  return true;
}

bool LLVMi32Type::isPointer() const {
  return false;
}

bool LLVMi32Type::isImmutable() const {
  return false;
}

void LLVMi32Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi32Type::createLocalVariable(IRGenerationContext& context,
                                      string name,
                                      int line) const {
  ILLVMType::createLocalLLVMVariable(context, this, name, line);
}

void LLVMi32Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object,
                                      int line) const {
  IVariable* variable = new FieldLLVMVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMi32Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value,
                                          int line) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMi32Type::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const LLVMPointerType* LLVMi32Type::getPointerType(IRGenerationContext& context, int line) const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi32Type::newTypeSpecifier(int line) const {
  return new LLVMPrimitiveTypeSpecifier(this, line);
}

Instruction* LLVMi32Type::inject(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
