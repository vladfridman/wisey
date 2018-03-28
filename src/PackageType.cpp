//
//  PackageType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayType.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/PackageType.hpp"

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

bool PackageType::isThread() const {
  return false;
}

bool PackageType::isNative() const {
  return false;
}

void PackageType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void PackageType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void PackageType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  assert(false);
}

void PackageType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  assert(false);
}

const wisey::ArrayType* PackageType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IReferenceType* PackageType::getReferenceType() const {
  return NULL;
}

const ILLVMPointerType* PackageType::getPointerType() const {
  assert(false);
}
