//
//  StringFormatType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringFormatType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

StringFormatType::StringFormatType() {
}

StringFormatType::~StringFormatType() {
}

string StringFormatType::getTypeName() const {
  return "stringformat";
}

Type* StringFormatType::getLLVMType(IRGenerationContext& cotnext) const {
  assert(false && "Calling getLLVMType on stringformat type expression");
}

bool StringFormatType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::STRING_FORMAT;
}

bool StringFormatType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::STRING_FORMAT;
}

Value* StringFormatType::castTo(IRGenerationContext& context,
                                Value* fromValue,
                                const IType* toType,
                                int line) const {
  if (toType == PrimitiveTypes::STRING_FORMAT) {
    return fromValue;
  }
  
  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool StringFormatType::isPrimitive() const {
  return true;
}

bool StringFormatType::isOwner() const {
  return false;
}

bool StringFormatType::isReference() const {
  return false;
}

bool StringFormatType::isArray() const {
  return false;
}

bool StringFormatType::isFunction() const {
  return false;
}

bool StringFormatType::isPackage() const {
  return false;
}

bool StringFormatType::isController() const {
  return false;
}

bool StringFormatType::isInterface() const {
  return false;
}

bool StringFormatType::isModel() const {
  return false;
}

bool StringFormatType::isNode() const {
  return false;
}

bool StringFormatType::isNative() const {
  return false;
}

bool StringFormatType::isPointer() const {
  return false;
}

bool StringFormatType::isImmutable() const {
  return false;
}

void StringFormatType::printToStream(IRGenerationContext &context, iostream& stream) const {
  assert(false && "Attempting to print stringformat type");
}

void StringFormatType::createLocalVariable(IRGenerationContext& context,
                                           string name,
                                           int line) const {
  assert(false && "Attempting to create a local variable of stringformat type");
}

void StringFormatType::createFieldVariable(IRGenerationContext& context,
                                           string name,
                                           const IConcreteObjectType* object,
                                           int line) const {
  assert(false && "Attempting to create a field variable of stringformat type");
}

void StringFormatType::createParameterVariable(IRGenerationContext& context,
                                               string name,
                                               Value* value,
                                               int line) const {
  assert(false && "Attempting to create a parameter variable of stringformat type");
}

const wisey::ArrayType* StringFormatType::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  exit(1);
}

Instruction* StringFormatType::inject(IRGenerationContext& context,
                                      const InjectionArgumentList injectionArgumentList,
                                      int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}

