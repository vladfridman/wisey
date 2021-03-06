//
//  VoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ArrayType.hpp"
#include "Cast.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string VoidType::getTypeName() const {
  return "void";
}

llvm::Type* VoidType::getLLVMType(IRGenerationContext& context) const {
  return Type::getVoidTy(context.getLLVMContext());
}

bool VoidType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::VOID;
}

bool VoidType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::VOID;
}

Value* VoidType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType == PrimitiveTypes::VOID) {
    return fromValue;
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool VoidType::isPrimitive() const {
  return true;
}

bool VoidType::isOwner() const {
  return false;
}

bool VoidType::isReference() const {
  return false;
}

bool VoidType::isArray() const {
  return false;
}

bool VoidType::isFunction() const {
  return false;
}

bool VoidType::isPackage() const {
  return false;
}

bool VoidType::isController() const {
  return false;
}

bool VoidType::isInterface() const {
  return false;
}

bool VoidType::isModel() const {
  return false;
}

bool VoidType::isNode() const {
  return false;
}

bool VoidType::isNative() const {
  return false;
}

bool VoidType::isPointer() const {
  return false;
}

bool VoidType::isImmutable() const {
  return false;
}

string VoidType::getFormat() const {
  return "";
}

void VoidType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void VoidType::createLocalVariable(IRGenerationContext& context,
                                   string name,
                                   int line) const {
  assert(false);
}

void VoidType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object,
                                   int line) const {
  assert(false);
}

void VoidType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value,
                                       int line) const {
  assert(false);
}

const wisey::ArrayType* VoidType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* VoidType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* VoidType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
