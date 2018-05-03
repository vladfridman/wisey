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
#include "wisey/CastObjectFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
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
  if (toType->isPointer()) {
    return true;
  }
  if (toType->isReference()) {
    return !toType->isController() && !toType->isNode();
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }
  return false;
}

bool WiseyModelType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* WiseyModelType::castTo(IRGenerationContext& context,
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
  return true;
}

bool WiseyModelType::isNode() const {
  return false;
}

bool WiseyModelType::isNative() const {
  return true;
}

bool WiseyModelType::isPointer() const {
  return false;
}

bool WiseyModelType::isImmutable() const {
  return true;
}

void WiseyModelType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void WiseyModelType::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca, line);
  context.getScopes().setVariable(uninitializedVariable);
}

void WiseyModelType::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  IVariable* variable = new FieldReferenceVariable(name, object, line);
  context.getScopes().setVariable(variable);
}

void WiseyModelType::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value,
                                             int line) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* WiseyModelType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
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

Instruction* WiseyModelType::inject(IRGenerationContext& context,
                                    const InjectionArgumentList injectionArgumentList,
                                    int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
