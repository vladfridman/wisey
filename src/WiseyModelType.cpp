//
//  TestWiseyModelType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
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
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyModelType* WiseyModelType::WISEY_MODEL_TYPE = new WiseyModelType();

WiseyModelType::WiseyModelType() {
}

WiseyModelType::~WiseyModelType() {
}

string WiseyModelType::getTypeName() const {
  return "::wisey::model";
}

PointerType* WiseyModelType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool WiseyModelType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isPointer();
}

bool WiseyModelType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType->isReference() || toType->isPointer();
}

Value* WiseyModelType::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType->isReference() || toType->isPointer()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  assert(false);
}

bool WiseyModelType::isPrimitive() const {
  return false;
}

bool WiseyModelType::isOwner() const {
  return false;
}

bool WiseyModelType::isReference() const {
  return true;
}

bool WiseyModelType::isArray() const {
  return false;
}

bool WiseyModelType::isFunction() const {
  return false;
}

bool WiseyModelType::isPackage() const {
  return false;
}

bool WiseyModelType::isController() const {
  return false;
}

bool WiseyModelType::isInterface() const {
  return false;
}

bool WiseyModelType::isModel() const {
  return false;
}

bool WiseyModelType::isNode() const {
  return false;
}

bool WiseyModelType::isThread() const {
  return false;
}

bool WiseyModelType::isNative() const {
  return true;
}

bool WiseyModelType::isPointer() const {
  return false;
}

void WiseyModelType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyModelType::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void WiseyModelType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void WiseyModelType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyModelType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

void WiseyModelType::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, 1);
}

void WiseyModelType::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, -1);
}

const IOwnerType* WiseyModelType::getOwner() const {
  return WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE;
}
