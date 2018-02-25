//
//  LongType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/LongType.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string LongType::getTypeName() const {
  return "long";
}

llvm::Type* LongType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt64Ty(context.getLLVMContext());
}

TypeKind LongType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool LongType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool LongType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::LONG_TYPE;
}

bool LongType::isPrimitive() const {
  return true;
}

bool LongType::isOwner() const {
  return false;
}

bool LongType::isReference() const {
  return false;
}

Value* LongType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::LONG_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

string LongType::getFormat() const {
  return "%d";
}

void LongType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

bool LongType::isArray() const {
  return false;
}

bool LongType::isFunction() const {
  return false;
}

bool LongType::isController() const {
  return false;
}

bool LongType::isInterface() const {
  return false;
}

bool LongType::isModel() const {
  return false;
}

bool LongType::isNode() const {
  return false;
}

void LongType::createLocalVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void LongType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LongType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* LongType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* LongType::getObjectType() const {
  return NULL;
}
