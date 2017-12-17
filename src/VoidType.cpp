//
//  VoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

VoidType::VoidType() {
}

VoidType::~VoidType() {
}

string VoidType::getTypeName() const {
  return "void";
}

llvm::Type* VoidType::getLLVMType(IRGenerationContext& context) const {
  return Type::getVoidTy(context.getLLVMContext());
}

TypeKind VoidType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool VoidType::canCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

bool VoidType::canAutoCastTo(const IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
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

const ArrayElementType* VoidType::getArrayElementType() const {
  Log::e("Should not be getting array element type of void type");
  exit(1);
}

string VoidType::getFormat() const {
  return "";
}
