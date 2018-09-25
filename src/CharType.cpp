//
//  CharType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "Cast.hpp"
#include "CharType.hpp"
#include "FieldPrimitiveVariable.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "ParameterPrimitiveVariable.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

CharType::CharType() {
}

CharType::~CharType() {
}

string CharType::getTypeName() const {
  return "char";
}

llvm::Type* CharType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext());
}

bool CharType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool CharType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
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

Value* CharType::castTo(IRGenerationContext& context,
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
  } else if (toType == PrimitiveTypes::CHAR || toType == PrimitiveTypes::BYTE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::INT || toType == PrimitiveTypes::LONG) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT || toType == PrimitiveTypes::DOUBLE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool CharType::isPrimitive() const {
  return true;
}

bool CharType::isOwner() const {
  return false;
}

bool CharType::isReference() const {
  return false;
}

bool CharType::isArray() const {
  return false;
}

bool CharType::isFunction() const {
  return false;
}

bool CharType::isPackage() const {
  return false;
}

bool CharType::isController() const {
  return false;
}

bool CharType::isInterface() const {
  return false;
}

bool CharType::isModel() const {
  return false;
}

bool CharType::isNode() const {
  return false;
}

bool CharType::isNative() const {
  return false;
}

bool CharType::isPointer() const {
  return false;
}

bool CharType::isImmutable() const {
  return false;
}

string CharType::getFormat() const {
  return "%c";
}

void CharType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void CharType::createLocalVariable(IRGenerationContext& context,
                                   string name,
                                   int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void CharType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object,
                                   int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void CharType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value,
                                       int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* CharType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* CharType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* CharType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
