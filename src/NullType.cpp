//
//  NullType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string NullType::getTypeName() const {
  return "null";
}

Type* NullType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

Value* NullType::computeSize(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 8);
}

TypeKind NullType::getTypeKind() const {
  return NULL_TYPE_KIND;
}

bool NullType::canCastTo(const IType* toType) const {
  return !IType::isPrimitveType(toType);
}

bool NullType::canAutoCastTo(const IType* toType) const {
  return !IType::isPrimitveType(toType);
}

Value* NullType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  return ConstantExpr::getNullValue(toType->getLLVMType(context));
}

bool NullType::isOwner() const {
  return false;
}

NullType* NullType::NULL_TYPE = new NullType();
