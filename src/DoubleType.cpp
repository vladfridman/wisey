//
//  DoubleType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/DoubleType.hpp"
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

DoubleType::DoubleType() {
}

DoubleType::~DoubleType() {
}

string DoubleType::getTypeName() const {
  return "double";
}

llvm::Type* DoubleType::getLLVMType(IRGenerationContext& context) const {
  return Type::getDoubleTy(context.getLLVMContext());
}

bool DoubleType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID && toType != PrimitiveTypes::STRING;
}

bool DoubleType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::DOUBLE;
}

Value* DoubleType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  } else if (toType == PrimitiveTypes::BOOLEAN ||
      toType == PrimitiveTypes::CHAR ||
      toType == PrimitiveTypes::INT ||
      toType == PrimitiveTypes::LONG) {
    return Cast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT) {
    return Cast::truncFloatCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::DOUBLE) {
    return fromValue;
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool DoubleType::isPrimitive() const {
  return true;
}

bool DoubleType::isOwner() const {
  return false;
}

bool DoubleType::isReference() const {
  return false;
}

bool DoubleType::isArray() const {
  return false;
}

bool DoubleType::isFunction() const {
  return false;
}

bool DoubleType::isPackage() const {
  return false;
}

bool DoubleType::isController() const {
  return false;
}

bool DoubleType::isInterface() const {
  return false;
}

bool DoubleType::isModel() const {
  return false;
}

bool DoubleType::isNode() const {
  return false;
}

bool DoubleType::isNative() const {
  return false;
}

bool DoubleType::isPointer() const {
  return false;
}

bool DoubleType::isImmutable() const {
  return false;
}

string DoubleType::getFormat() const {
  return "%e";
}

void DoubleType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void DoubleType::createLocalVariable(IRGenerationContext& context,
                                     string name,
                                     int line) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantFP::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void DoubleType::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object,
                                     int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(variable);
}

void DoubleType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value,
                                         int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* DoubleType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

const PrimitiveTypeSpecifier* DoubleType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* DoubleType::inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
