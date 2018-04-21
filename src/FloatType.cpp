//
//  FloatType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/FloatType.hpp"
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

FloatType::FloatType() {
}

FloatType::~FloatType() {
}

string FloatType::getTypeName() const {
  return "float";
}

llvm::Type* FloatType::getLLVMType(IRGenerationContext& context) const {
  return Type::getFloatTy(context.getLLVMContext());
}

bool FloatType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool FloatType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return true;
  }
  if (!toType->isPrimitive()) {
    return false;
  }
  
  return toType == PrimitiveTypes::FLOAT_TYPE || toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* FloatType::castTo(IRGenerationContext& context,
                         Value* fromValue,
                         const IType* toType,
                         int line) const {
  if (toType->isNative() && toType->getLLVMType(context) == getLLVMType(context)) {
    return fromValue;
  } else if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return fromValue;
  } else if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return Cast::widenFloatCast(context, fromValue, toType);
  }
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool FloatType::isPrimitive() const {
  return true;
}

bool FloatType::isOwner() const {
  return false;
}

bool FloatType::isReference() const {
  return false;
}

bool FloatType::isFunction() const {
  return false;
}

bool FloatType::isArray() const {
  return false;
}

bool FloatType::isPackage() const {
  return false;
}

bool FloatType::isController() const {
  return false;
}

bool FloatType::isInterface() const {
  return false;
}

bool FloatType::isModel() const {
  return false;
}

bool FloatType::isNode() const {
  return false;
}

bool FloatType::isThread() const {
  return false;
}

bool FloatType::isNative() const {
  return false;
}

bool FloatType::isPointer() const {
  return false;
}

bool FloatType::isImmutable() const {
  return false;
}

string FloatType::getFormat() const {
  return "%f";
}

void FloatType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void FloatType::createLocalVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantFP::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}

void FloatType::createFieldVariable(IRGenerationContext& context,
                                    string name,
                                    const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void FloatType::createParameterVariable(IRGenerationContext& context,
                                        string name,
                                        Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* FloatType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const PrimitiveTypeSpecifier* FloatType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* FloatType::inject(IRGenerationContext& context,
                               const InjectionArgumentList injectionArgumentList,
                               int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
