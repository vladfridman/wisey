//
//  VoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayType.hpp"
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

llvm::Type* VoidType::getLLVMType(IRGenerationContext& context) const {
  return Type::getVoidTy(context.getLLVMContext());
}

TypeKind VoidType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool VoidType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

bool VoidType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
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

bool VoidType::isReference() const {
  return false;
}

string VoidType::getFormat() const {
  return "";
}

void VoidType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void VoidType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void VoidType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object) const {
  assert(false);
}

const wisey::ArrayType* VoidType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
