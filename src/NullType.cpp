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

TypeKind NullType::getTypeKind() const {
  return NULL_TYPE_KIND;
}

bool NullType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return !IType::isPrimitveType(toType);
}

bool NullType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
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

bool NullType::isReference() const {
  return false;
}

void NullType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void NullType::allocateLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

const wisey::ArrayType* NullType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

NullType* NullType::NULL_TYPE = new NullType();
