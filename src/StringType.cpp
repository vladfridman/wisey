//
//  StringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/Cast.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string StringType::getTypeName() const {
  return "string";
}

llvm::Type* StringType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

TypeKind StringType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool StringType::canCastTo(const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

bool StringType::canAutoCastTo(const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

Value* StringType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == PrimitiveTypes::STRING_TYPE) {
    return fromValue;
  }

  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool StringType::isOwner() const {
  return false;
}

string StringType::getFormat() const {
  return "%s";
}

void StringType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}
