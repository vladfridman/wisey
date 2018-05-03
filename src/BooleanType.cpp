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
}

BooleanType::~BooleanType() {
}

string BooleanType::getTypeName() const {
  return "boolean";
}

llvm::Type* BooleanType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt1Ty(context.getLLVMContext());
}

bool BooleanType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool BooleanType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::BOOLEAN ||
    toType == PrimitiveTypes::CHAR ||
    toType == PrimitiveTypes::INT ||
    toType == PrimitiveTypes::LONG ||
    toType == PrimitiveTypes::FLOAT ||
    toType == PrimitiveTypes::DOUBLE;
}

Value* BooleanType::castTo(IRGenerationContext& context,
                           Value* fromValue,
                           const IType* toType,
                           int line) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  } else if (toType == PrimitiveTypes::BOOLEAN) {
    return fromValue;
  } else if (toType == PrimitiveTypes::CHAR ||
             toType == PrimitiveTypes::INT ||
             toType == PrimitiveTypes::LONG) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT || toType == PrimitiveTypes::DOUBLE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
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

bool BooleanType::isNative() const {
  return false;
}

bool BooleanType::isPointer() const {
  return false;
}

bool BooleanType::isImmutable() const {
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

const wisey::ArrayType* BooleanType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const PrimitiveTypeSpecifier* BooleanType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* BooleanType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
