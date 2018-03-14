//
//  NativeVoidPointerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalNativeVariable.hpp"
#include "wisey/NativeVoidPointerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NativeVoidPointerType::NativeVoidPointerType() {
}

NativeVoidPointerType::~NativeVoidPointerType() {
}

string NativeVoidPointerType::getTypeName() const {
  return "native:i8*";
}

llvm::PointerType* NativeVoidPointerType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool NativeVoidPointerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this;
}

bool NativeVoidPointerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == this;
}

Value* NativeVoidPointerType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

bool NativeVoidPointerType::isPrimitive() const {
  return false;
}

bool NativeVoidPointerType::isOwner() const {
  return false;
}

bool NativeVoidPointerType::isReference() const {
  return true;
}

bool NativeVoidPointerType::isArray() const {
  return false;
}

bool NativeVoidPointerType::isFunction() const {
  return false;
}

bool NativeVoidPointerType::isPackage() const {
  return false;
}

bool NativeVoidPointerType::isController() const {
  return false;
}

bool NativeVoidPointerType::isInterface() const {
  return false;
}

bool NativeVoidPointerType::isModel() const {
  return false;
}

bool NativeVoidPointerType::isNode() const {
  return false;
}

bool NativeVoidPointerType::isThread() const {
  return false;
}

bool NativeVoidPointerType::isNative() const {
  return true;
}

void NativeVoidPointerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  assert(false);
}

void NativeVoidPointerType::createLocalVariable(IRGenerationContext& context, string name) const {
  Value* alloca = IRWriter::newAllocaInst(context, getLLVMType(context), name);
  IVariable* variable = new LocalNativeVariable(name, this, alloca);
  context.getScopes().setVariable(variable);
}

void NativeVoidPointerType::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  assert(false);
}

void NativeVoidPointerType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  assert(false);
}

const wisey::ArrayType* NativeVoidPointerType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* NativeVoidPointerType::getObjectType() const {
  return NULL;
}

const wisey::PointerType* NativeVoidPointerType::getPointerType() const {
  assert(false);
}
