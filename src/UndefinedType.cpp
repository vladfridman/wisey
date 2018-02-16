//
//  UndefinedType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string UndefinedType::getTypeName() const {
  return "undefined";
}

Type* UndefinedType::getLLVMType(IRGenerationContext& context) const {
  return NULL;
}

TypeKind UndefinedType::getTypeKind() const {
  return UNDEFINED_TYPE_KIND;
}

bool UndefinedType::canCastTo(const IType* toType) const {
  return false;
}

bool UndefinedType::canAutoCastTo(const IType* toType) const {
  return false;
}

Value* UndefinedType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  return NULL;
}

bool UndefinedType::isOwner() const {
  return false;
}

void UndefinedType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

UndefinedType* UndefinedType::UNDEFINED_TYPE = new UndefinedType();

