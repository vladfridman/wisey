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
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BooleanType::BooleanType() {
  mPointerType = new PointerType(this);
}

BooleanType::~BooleanType() {
  delete mPointerType;
}

string BooleanType::getTypeName() const {
  return "boolean";
}

llvm::Type* BooleanType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt1Ty(context.getLLVMContext());
}

bool BooleanType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool BooleanType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (!toType->isPrimitive()) {
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

bool BooleanType::isPrimitive() const {
  return true;
}

bool BooleanType::isOwner() const {
  return false;
}

bool BooleanType::isReference() const {
  return false;
}

bool BooleanType::isArray() const {
  return false;
}

bool BooleanType::isFunction() const {
  return false;
}

bool BooleanType::isPackage() const {
  return false;
}

bool BooleanType::isController() const {
  return false;
}

bool BooleanType::isInterface() const {
  return false;
}

bool BooleanType::isModel() const {
  return false;
}

bool BooleanType::isNode() const {
  return false;
}

bool BooleanType::isThread() const {
  return false;
}

bool BooleanType::isNative() const {
  return false;
}

string BooleanType::getFormat() const {
  return "%d";
}

void BooleanType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void BooleanType::createLocalVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void BooleanType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void BooleanType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* BooleanType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IObjectType* BooleanType::getObjectType() const {
  return NULL;
}

const IType* BooleanType::getPointerType() const {
  return mPointerType;
}

const IType* BooleanType::getDereferenceType() const {
  assert(false);
}

PrimitiveTypeSpecifier* BooleanType::newTypeSpecifier() const {
  return new PrimitiveTypeSpecifier(this);
}
