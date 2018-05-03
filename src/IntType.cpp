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
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntType.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IntType::IntType() {
}

IntType::~IntType() {
}

string IntType::getTypeName() const {
  return "int";
}

llvm::Type* IntType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt32Ty(context.getLLVMContext());
}

bool IntType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool IntType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::INT ||
    toType == PrimitiveTypes::LONG ||
    toType == PrimitiveTypes::DOUBLE;
}

Value* IntType::castTo(IRGenerationContext& context,
                       Value* fromValue,
                       const IType* toType,
                       int line) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  } else if (toType == PrimitiveTypes::BOOLEAN || toType == PrimitiveTypes::CHAR) {
    return Cast::truncIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::INT) {
    return fromValue;
  } else if (toType == PrimitiveTypes::LONG) {
    return Cast::widenIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT || toType == PrimitiveTypes::DOUBLE) {
    return Cast::intToFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool IntType::isPrimitive() const {
  return true;
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

bool IntType::isFunction() const {
  return false;
}

bool IntType::isPackage() const {
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

bool IntType::isNative() const {
  return false;
}

bool IntType::isPointer() const {
  return false;
}

bool IntType::isImmutable() const {
  return false;
}

string IntType::getFormat() const {
  return "%d";
}

void IntType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void IntType::createLocalVariable(IRGenerationContext& context,
                                  string name,
                                  int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void IntType::createFieldVariable(IRGenerationContext& context,
                                  string name,
                                  const IConcreteObjectType* object,
                                  int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(variable);
}

void IntType::createParameterVariable(IRGenerationContext& context,
                                      string name,
                                      Value* value,
                                      int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* IntType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const PrimitiveTypeSpecifier* IntType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* IntType::inject(IRGenerationContext& context,
                             const InjectionArgumentList injectionArgumentList,
                             int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
