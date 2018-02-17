//
//  BooleanType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/BooleanType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string BooleanType::getTypeName() const {
  return "boolean";
}

llvm::Type* BooleanType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt1Ty(context.getLLVMContext());
}

TypeKind BooleanType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool BooleanType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool BooleanType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType == PrimitiveTypes::BOOLEAN_TYPE ||
    toType == PrimitiveTypes::CHAR_TYPE ||
    toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::FLOAT_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* BooleanType::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::CHAR_TYPE ||
             toType == PrimitiveTypes::INT_TYPE ||
             toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool BooleanType::isOwner() const {
  return false;
}

bool BooleanType::isReference() const {
  return false;
}

string BooleanType::getFormat() const {
  return "%d";
}

void BooleanType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void BooleanType::allocateVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

const wisey::ArrayType* BooleanType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

