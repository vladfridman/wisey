//
//  IntType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntType.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string IntType::getTypeName() const {
  return "int";
}

llvm::Type* IntType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt32Ty(context.getLLVMContext());
}

TypeKind IntType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool IntType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool IntType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType == PrimitiveTypes::INT_TYPE ||
    toType == PrimitiveTypes::LONG_TYPE ||
    toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* IntType::castTo(IRGenerationContext& context,
                       Value* fromValue,
                       const IType* toType,
                       int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE || toType == PrimitiveTypes::CHAR_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::INT_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool IntType::isOwner() const {
  return false;
}

bool IntType::isReference() const {
  return false;
}

bool IntType::isArray() const {
  return false;
}

bool IntType::isController() const {
  return false;
}

bool IntType::isInterface() const {
  return false;
}

bool IntType::isModel() const {
  return false;
}

bool IntType::isNode() const {
  return false;
}

string IntType::getFormat() const {
  return "%d";
}

void IntType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void IntType::createLocalVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void IntType::createFieldVariable(IRGenerationContext& context,
                                  string name,
                                  const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void IntType::createParameterVariable(IRGenerationContext& context,
                                      string name,
                                      Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* IntType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
