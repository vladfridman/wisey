//
//  ArrayElementType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayElementType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ArrayElementType::ArrayElementType(const IType* baseType) : mBaseType(baseType) {
}

ArrayElementType::~ArrayElementType() {
}

const IType* ArrayElementType::getBaseType() const {
  return mBaseType;
}

string ArrayElementType::getTypeName() const {
  return "ArrayElement of type " + mBaseType->getTypeName();
}

Type* ArrayElementType::getLLVMType(IRGenerationContext& context) const {
  return mBaseType->getLLVMType(context)->getPointerTo();
}

TypeKind ArrayElementType::getTypeKind() const {
  return ARRAY_ELEMENT_TYPE;
}

bool ArrayElementType::canCastTo(const IType* toType) const {
  return mBaseType->canCastTo(toType);
}

bool ArrayElementType::canAutoCastTo(const IType* toType) const {
  return mBaseType->canAutoCastTo(toType);
}

Value* ArrayElementType::castTo(IRGenerationContext& context,
                                Value* fromValue,
                                const IType* toType,
                                int line) const {
  Value* value = IRWriter::newLoadInst(context, fromValue, "");
  return mBaseType->castTo(context, value, toType, line);
}

ArrayElementType* ArrayElementType::getArrayElementType() const {
  Log::e("Can not get an array element of another array element");
  exit(1);
  return NULL;
}
