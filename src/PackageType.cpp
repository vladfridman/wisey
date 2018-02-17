//
//  PackageType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

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

TypeKind PackageType::getTypeKind() const {
  return PACKAGE_TYPE;
}

bool PackageType::canCastTo(const IType *toType) const {
  return false;
}

bool PackageType::canAutoCastTo(const IType *toType) const {
  return false;
}

Value* PackageType::castTo(IRGenerationContext &context,
                           Value* fromValue,
                           const wisey::IType* toType,
                           int line) const {
  return NULL;
}

bool PackageType::isOwner() const {
  return false;
}

void PackageType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void PackageType::allocateVariable(IRGenerationContext& context, string name) const {
  assert(false);
}
