//
//  UndefinedType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "ArrayType.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "UndefinedType.hpp"

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

bool UndefinedType::isNative() const {
  return false;
}

bool UndefinedType::isPointer() const {
  return false;
}

bool UndefinedType::isImmutable() const {
  return false;
}

void UndefinedType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void UndefinedType::createLocalVariable(IRGenerationContext& context,
                                        string name,
                                        int line) const {
  assert(false);
}

void UndefinedType::createFieldVariable(IRGenerationContext& context,
                                        string name,
                                        const IConcreteObjectType* object,
                                        int line) const {
  assert(false);
}

void UndefinedType::createParameterVariable(IRGenerationContext& context,
                                            string name,
                                            Value* value,
                                            int line) const {
  assert(false);
}

const wisey::ArrayType* UndefinedType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* UndefinedType::inject(IRGenerationContext& context,
                                   const InjectionArgumentList injectionArgumentList,
                                   int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

const UndefinedType* UndefinedType::UNDEFINED = new UndefinedType();

