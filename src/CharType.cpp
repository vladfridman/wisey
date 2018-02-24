//
//  CharType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/CharType.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string CharType::getTypeName() const {
  return "char";
}

llvm::Type* CharType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt16Ty(context.getLLVMContext());
}

TypeKind CharType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool CharType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool CharType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }

  return toType == PrimitiveTypes::CHAR_TYPE ||
    toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::FLOAT_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* CharType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::CHAR_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::INT_TYPE || toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool CharType::isOwner() const {
  return false;
}

bool CharType::isReference() const {
  return false;
}

string CharType::getFormat() const {
  return "%c";
}

void CharType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void CharType::createLocalVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void CharType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void CharType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* CharType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
