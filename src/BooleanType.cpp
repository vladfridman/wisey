//
//  BooleanType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "BooleanType.hpp"
#include "Cast.hpp"
#include "FieldPrimitiveVariable.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "ParameterPrimitiveVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

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
    toType == PrimitiveTypes::BYTE ||
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
             toType == PrimitiveTypes::BYTE ||
             toType == PrimitiveTypes::INT ||
             toType == PrimitiveTypes::LONG) {
    return IRWriter::createZExtOrBitCast(context, fromValue, toType->getLLVMType(context));
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

void BooleanType::createLocalVariable(IRGenerationContext& context,
                                      string name,
                                      int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void BooleanType::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object,
                                      int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void BooleanType::createParameterVariable(IRGenerationContext& context,
                                          string name,
                                          Value* value,
                                          int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* BooleanType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* BooleanType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* BooleanType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
