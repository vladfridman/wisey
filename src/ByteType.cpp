//
//  ByteType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/ByteType.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ByteType::ByteType() {
}

ByteType::~ByteType() {
}

string ByteType::getTypeName() const {
  return "byte";
}

llvm::Type* ByteType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext());
}

bool ByteType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool ByteType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
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

Value* ByteType::castTo(IRGenerationContext& context,
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

bool ByteType::isPrimitive() const {
  return true;
}

bool ByteType::isOwner() const {
  return false;
}

bool ByteType::isReference() const {
  return false;
}

bool ByteType::isArray() const {
  return false;
}

bool ByteType::isFunction() const {
  return false;
}

bool ByteType::isPackage() const {
  return false;
}

bool ByteType::isController() const {
  return false;
}

bool ByteType::isInterface() const {
  return false;
}

bool ByteType::isModel() const {
  return false;
}

bool ByteType::isNode() const {
  return false;
}

bool ByteType::isNative() const {
  return false;
}

bool ByteType::isPointer() const {
  return false;
}

bool ByteType::isImmutable() const {
  return false;
}

string ByteType::getFormat() const {
  return "%d";
}

void ByteType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ByteType::createLocalVariable(IRGenerationContext& context,
                                   string name,
                                   int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void ByteType::createFieldVariable(IRGenerationContext& context,
                                   string name,
                                   const IConcreteObjectType* object,
                                   int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void ByteType::createParameterVariable(IRGenerationContext& context,
                                       string name,
                                       Value* value,
                                       int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* ByteType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* ByteType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* ByteType::inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
