//
//  PackageType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ArrayType.hpp"
#include "IObjectType.hpp"
#include "PackageType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

PackageType::PackageType(string packageName) : mPackageName(packageName) {
}

PackageType::~PackageType() {
}

string PackageType::getTypeName() const {
  return mPackageName;
}

Type* PackageType::getLLVMType(IRGenerationContext &context) const {
  return NULL;
}

bool PackageType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

bool PackageType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* PackageType::castTo(IRGenerationContext &context,
                           Value* fromValue,
                           const wisey::IType* toType,
                           int line) const {
  return NULL;
}

bool PackageType::isPrimitive() const {
  return false;
}

bool PackageType::isOwner() const {
  return false;
}

bool PackageType::isReference() const {
  return false;
}

bool PackageType::isArray() const {
  return false;
}

bool PackageType::isFunction() const {
  return false;
}

bool PackageType::isPackage() const {
  return true;
}

bool PackageType::isController() const {
  return false;
}

bool PackageType::isInterface() const {
  return false;
}

bool PackageType::isModel() const {
  return false;
}

bool PackageType::isNode() const {
  return false;
}

bool PackageType::isNative() const {
  return false;
}

bool PackageType::isPointer() const {
  return false;
}

bool PackageType::isImmutable() const {
  return false;
}

void PackageType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void PackageType::createLocalVariable(IRGenerationContext& context,
                                      string name,
                                      int line) const {
  assert(false);
}

void PackageType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object,
                                      int line) const {
  assert(false);
}

void PackageType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value,
                                          int line) const {
  assert(false);
}

const wisey::ArrayType* PackageType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* PackageType::inject(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
