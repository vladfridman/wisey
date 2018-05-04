//
//  WiseyModelOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/WiseyModelOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyModelOwnerType* WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE = new WiseyModelOwnerType();

WiseyModelOwnerType::WiseyModelOwnerType() {
}

WiseyModelOwnerType::~WiseyModelOwnerType() {
}

string WiseyModelOwnerType::getTypeName() const {
  return getReference()->getTypeName() + "*";
}

llvm::PointerType* WiseyModelOwnerType::getLLVMType(IRGenerationContext& context) const {
  return getReference()->getLLVMType(context);
}

bool WiseyModelOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isPointer()) {
    return true;
  }
  if (toType->isReference() || toType->isOwner()) {
    return !toType->isController() && !toType->isNode();
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }
  return false;
}

bool WiseyModelOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* WiseyModelOwnerType::castTo(IRGenerationContext& context,
                                         llvm::Value* fromValue,
                                         const IType* toType,
                                         int line) const {
  if (toType->isNative() && (toType->isReference() || toType->isOwner() || toType->isPointer())) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 ICmpInst::ICMP_NE,
                                 fromValue,
                                 ConstantPointerNull::get(getLLVMType(context)),
                                 "");
  }

  assert(IObjectType::isObjectType(toType));
  if (toType->isOwner()) {
    const IReferenceType* toReferenceType = ((const IOwnerType*) toType)->getReference();
    return getReference()->castTo(context, fromValue, toReferenceType, line);
  }
  
  return getReference()->castTo(context, fromValue, toType, line);
}

bool WiseyModelOwnerType::isPrimitive() const {
  return false;
}

bool WiseyModelOwnerType::isOwner() const {
  return true;
}

bool WiseyModelOwnerType::isReference() const {
  return false;
}

bool WiseyModelOwnerType::isArray() const {
  return false;
}

bool WiseyModelOwnerType::isFunction() const {
  return false;
}

bool WiseyModelOwnerType::isPackage() const {
  return false;
}

bool WiseyModelOwnerType::isController() const {
  return false;
}

bool WiseyModelOwnerType::isInterface() const {
  return false;
}

bool WiseyModelOwnerType::isModel() const {
  return true;
}

bool WiseyModelOwnerType::isNode() const {
  return false;
}

bool WiseyModelOwnerType::isNative() const {
  return true;
}

bool WiseyModelOwnerType::isPointer() const {
  return false;
}

bool WiseyModelOwnerType::isImmutable() const {
  return true;
}

void WiseyModelOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyModelOwnerType::createLocalVariable(IRGenerationContext& context,
                                              string name,
                                              int line) const {
  PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, variable);
}

void WiseyModelOwnerType::createFieldVariable(IRGenerationContext& context,
                                              string name,
                                              const IConcreteObjectType* object,
                                              int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void WiseyModelOwnerType::createParameterVariable(IRGenerationContext& context,
                                                  string name,
                                                  llvm::Value* value,
                                                  int line) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* WiseyModelOwnerType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const WiseyModelType* WiseyModelOwnerType::getReference() const {
  return WiseyModelType::WISEY_MODEL_TYPE;
}

void WiseyModelOwnerType::free(IRGenerationContext& context, Value* value, int line) const {
  IConcreteObjectType::composeDestructorCall(context, value, line);
}

Instruction* WiseyModelOwnerType::inject(IRGenerationContext& context,
                                         const InjectionArgumentList injectionArgumentList,
                                         int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
