//
//  VoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string VoidType::getTypeName() const {
  return "void";
}

Type* VoidType::getLLVMType(IRGenerationContext& context) const {
  return Type::getVoidTy(context.getLLVMContext());
}

Value* VoidType::computeSize(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
}

TypeKind VoidType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool VoidType::canCastTo(const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

bool VoidType::canAutoCastTo(const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
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

bool VoidType::isOwner() const {
  return false;
}

string VoidType::getFormat() const {
  return "";
}
