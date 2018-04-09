//
//  WiseyObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectType* WiseyObjectType::LLVM_OBJECT_TYPE = new WiseyObjectType();

WiseyObjectType::WiseyObjectType() {
}

WiseyObjectType::~WiseyObjectType() {
}

string WiseyObjectType::getTypeName() const {
  return "::wisey::object";
}

PointerType* WiseyObjectType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool WiseyObjectType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isPointer();
}

bool WiseyObjectType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isPointer();
}

Value* WiseyObjectType::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType->isReference() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool WiseyObjectType::isPrimitive() const {
  return false;
}

bool WiseyObjectType::isOwner() const {
  return false;
}

bool WiseyObjectType::isReference() const {
  return true;
}

bool WiseyObjectType::isArray() const {
  return false;
}

bool WiseyObjectType::isFunction() const {
  return false;
}

bool WiseyObjectType::isPackage() const {
  return false;
}

bool WiseyObjectType::isController() const {
  return false;
}

bool WiseyObjectType::isInterface() const {
  return false;
}

bool WiseyObjectType::isModel() const {
  return false;
}

bool WiseyObjectType::isNode() const {
  return false;
}

bool WiseyObjectType::isThread() const {
  return false;
}

bool WiseyObjectType::isNative() const {
  return true;
}

bool WiseyObjectType::isPointer() const {
  return false;
}

void WiseyObjectType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyObjectType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void WiseyObjectType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void WiseyObjectType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyObjectType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

void WiseyObjectType::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, 1);
}

void WiseyObjectType::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, -1);
}

const IOwnerType* WiseyObjectType::getOwner() const {
  return WiseyObjectOwnerType::LLVM_OBJECT_OWNER_TYPE;
}