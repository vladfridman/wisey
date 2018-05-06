//
//  LLVMi64Type.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMi64Type.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMi64Type::LLVMi64Type() {
  mPointerType = LLVMPointerType::create(this);
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

bool LLVMi64Type::isNative() const {
  return true;
}

bool LLVMi64Type::isPointer() const {
  return false;
}

bool LLVMi64Type::isImmutable() const {
  return false;
}

void LLVMi64Type::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMi64Type::createLocalVariable(IRGenerationContext& context,
                                      string name,
                                      int line) const {
  ILLVMType::createLocalLLVMVariable(context, this, name, line);
}

void LLVMi64Type::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object,
                                      int line) const {
  IVariable* variable = new FieldLLVMVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMi64Type::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value,
                                          int line) const {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMi64Type::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const LLVMPointerType* LLVMi64Type::getPointerType(IRGenerationContext& context, int line) const {
  return mPointerType;
}

const ILLVMTypeSpecifier* LLVMi64Type::newTypeSpecifier(int line) const {
  return new LLVMPrimitiveTypeSpecifier(this, line);
}

Instruction* LLVMi64Type::inject(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
