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
#include "wisey/CastObjectFunction.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectType* WiseyObjectType::WISEY_OBJECT_TYPE = new WiseyObjectType();

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
  if (toType->isPointer()) {
    return true;
  }
  if (toType->isReference()) {
    return true;
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }
  return false;
}

bool WiseyObjectType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* WiseyObjectType::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType->isNative() && (toType->isReference() || toType->isPointer())) {
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
  
  const IObjectType* toObjectType = (const IObjectType*) toType;
  return CastObjectFunction::call(context, fromValue, toObjectType, line);
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

bool WiseyObjectType::isNative() const {
  return true;
}

bool WiseyObjectType::isPointer() const {
  return false;
}

bool WiseyObjectType::isImmutable() const {
  return false;
}

void WiseyObjectType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyObjectType::createLocalVariable(IRGenerationContext& context,
                                          string name,
                                          int line) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca, line);
  context.getScopes().setVariable(uninitializedVariable);
}

void WiseyObjectType::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object,
                                          int line) const {
  IVariable* variable = new FieldReferenceVariable(name, object, line);
  context.getScopes().setVariable(variable);
}

void WiseyObjectType::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value,
                                              int line) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyObjectType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

void WiseyObjectType::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, 1);
}

void WiseyObjectType::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, -1);
}

const IOwnerType* WiseyObjectType::getOwner() const {
  return WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE;
}

Instruction* WiseyObjectType::inject(IRGenerationContext& context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
