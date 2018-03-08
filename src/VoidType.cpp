//
//  VoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/VoidType.hpp"

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
  return toType == PrimitiveTypes::VOID_TYPE;
}

bool VoidType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::VOID_TYPE;
}

Value* VoidType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType == PrimitiveTypes::VOID_TYPE) {
    return fromValue;
  }
  Cast::exitIncompatibleTypes(this, toType);
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

bool VoidType::isThread() const {
  return false;
}

bool VoidType::isNative() const {
  return false;
}

string VoidType::getFormat() const {
  return "";
}

void VoidType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void VoidType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void VoidType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object) const {
  assert(false);
}

void VoidType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value) const {
  assert(false);
}

const wisey::ArrayType* VoidType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* VoidType::getObjectType() const {
  return NULL;
}

