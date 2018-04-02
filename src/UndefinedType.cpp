//
//  UndefinedType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/ArrayType.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string UndefinedType::getTypeName() const {
  return "undefined";
}

Type* UndefinedType::getLLVMType(IRGenerationContext& context) const {
  return NULL;
}

bool UndefinedType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool UndefinedType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

Value* UndefinedType::castTo(IRGenerationContext& context,
                        Value* fromValue,
                        const IType* toType,
                        int line) const {
  return NULL;
}

bool UndefinedType::isPrimitive() const {
  return false;
}

bool UndefinedType::isOwner() const {
  return false;
}

bool UndefinedType::isReference() const {
  return false;
}

bool UndefinedType::isArray() const {
  return false;
}

bool UndefinedType::isFunction() const {
  return false;
}

bool UndefinedType::isPackage() const {
  return false;
}

bool UndefinedType::isController() const {
  return false;
}

bool UndefinedType::isInterface() const {
  return false;
}

bool UndefinedType::isModel() const {
  return false;
}

bool UndefinedType::isNode() const {
  return false;
}

bool UndefinedType::isThread() const {
  return false;
}

bool UndefinedType::isNative() const {
  return false;
}

void UndefinedType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void UndefinedType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void UndefinedType::createFieldVariable(IRGenerationContext& context,
                                        string name,
                                        const IConcreteObjectType* object) const {
  assert(false);
}

void UndefinedType::createParameterVariable(IRGenerationContext& context,
                                            string name,
                                            Value* value) const {
  assert(false);
}

const wisey::ArrayType* UndefinedType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const ILLVMPointerType* UndefinedType::getPointerType() const {
  assert(false);
}

UndefinedType* UndefinedType::UNDEFINED_TYPE = new UndefinedType();

