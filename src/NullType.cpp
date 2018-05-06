//
//  NullType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/ArrayType.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string NullType::getTypeName() const {
  return "null";
}

Type* NullType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool NullType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return !toType->isPrimitive();
}

bool NullType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return !toType->isPrimitive();
}

Value* NullType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  return ConstantExpr::getNullValue(toType->getLLVMType(context));
}

bool NullType::isPrimitive() const {
  return false;
}

bool NullType::isOwner() const {
  return false;
}

bool NullType::isReference() const {
  return false;
}

bool NullType::isArray() const {
  return false;
}

bool NullType::isFunction() const {
  return false;
}

bool NullType::isPackage() const {
  return false;
}

bool NullType::isController() const {
  return false;
}

bool NullType::isInterface() const {
  return false;
}

bool NullType::isModel() const {
  return false;
}

bool NullType::isNode() const {
  return false;
}

bool NullType::isNative() const {
  return false;
}

bool NullType::isPointer() const {
  return false;
}

bool NullType::isImmutable() const {
  return false;
}

void NullType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void NullType::createLocalVariable(IRGenerationContext& context,
                                   string name,
                                   int line) const {
  assert(false);
}

void NullType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object,
                                   int line) const {
  assert(false);
}

void NullType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value,
                                       int line) const {
  assert(false);
}

const wisey::ArrayType* NullType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* NullType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

NullType* NullType::NULL_TYPE = new NullType();
