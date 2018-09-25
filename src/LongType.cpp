//
//  LongType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "Cast.hpp"
#include "FieldPrimitiveVariable.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "LongType.hpp"
#include "ParameterPrimitiveVariable.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LongType::LongType() {
}

LongType::~LongType() {
}

string LongType::getTypeName() const {
  return "long";
}

llvm::Type* LongType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt64Ty(context.getLLVMContext());
}

bool LongType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool LongType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::BOOLEAN || toType == PrimitiveTypes::LONG;
}

Value* LongType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  } else if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 ICmpInst::ICMP_NE,
                                 fromValue,
                                 ConstantInt::get(getLLVMType(context), 0),
                                 "");
  } else if (toType == PrimitiveTypes::CHAR || toType == PrimitiveTypes::BYTE ||
             toType == PrimitiveTypes::INT) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::LONG) {
    return fromValue;
  } else if (toType == PrimitiveTypes::FLOAT || toType == PrimitiveTypes::DOUBLE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

string LongType::getFormat() const {
  return "%ld";
}

void LongType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
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

bool LongType::isArray() const {
  return false;
}

bool LongType::isFunction() const {
  return false;
}

bool LongType::isPackage() const {
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

bool LongType::isNative() const {
  return false;
}

bool LongType::isPointer() const {
  return false;
}

bool LongType::isImmutable() const {
  return false;
}

void LongType::createLocalVariable(IRGenerationContext& context,
                                   string name,
                                   int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void LongType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object,
                                   int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LongType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value,
                                       int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LongType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* LongType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* LongType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
