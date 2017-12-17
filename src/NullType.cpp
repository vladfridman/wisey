//
//  NullType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/ArrayElementType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NullType::NullType() {
}

NullType::~NullType() {
}

string NullType::getTypeName() const {
  return "null";
}

Type* NullType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

TypeKind NullType::getTypeKind() const {
  return NULL_TYPE_KIND;
}

bool NullType::canCastTo(const IType* toType) const {
  return toType->getTypeKind() != PRIMITIVE_TYPE;
}

bool NullType::canAutoCastTo(const IType* toType) const {
  return toType->getTypeKind() != PRIMITIVE_TYPE;
}

Value* NullType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  return ConstantExpr::getNullValue(toType->getLLVMType(context));
}

const ArrayElementType* NullType::getArrayElementType() const {
  Log::e("Should not be getting array element type of null type");
  exit(1);
}

NullType* NullType::NULL_TYPE = new NullType();
