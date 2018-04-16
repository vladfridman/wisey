//
//  StringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayType.hpp"
#include "wisey/Cast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

const string StringType::EMPTY_STRING_CONSTANT_NAME = "__empty.str";

StringType::StringType() {
}

StringType::~StringType() {
}

string StringType::getTypeName() const {
  return "string";
}

llvm::Type* StringType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool StringType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

bool StringType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

Value* StringType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == PrimitiveTypes::STRING_TYPE) {
    return fromValue;
  }

  Cast::exitIncompatibleTypes(context, this, toType, line);
  return NULL;
}

bool StringType::isPrimitive() const {
  return true;
}

bool StringType::isOwner() const {
  return false;
}

bool StringType::isReference() const {
  return false;
}

string StringType::getFormat() const {
  return "%s";
}

bool StringType::isArray() const {
  return false;
}

bool StringType::isFunction() const {
  return false;
}

bool StringType::isPackage() const {
  return false;
}

bool StringType::isController() const {
  return false;
}

bool StringType::isInterface() const {
  return false;
}

bool StringType::isModel() const {
  return false;
}

bool StringType::isNode() const {
  return false;
}

bool StringType::isThread() const {
  return false;
}

bool StringType::isNative() const {
  return false;
}

bool StringType::isPointer() const {
  return false;
}

void StringType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void StringType::createLocalVariable(IRGenerationContext& context, string name) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  GlobalVariable* emptyString = context.getModule()->getNamedGlobal(EMPTY_STRING_CONSTANT_NAME);

  if (!emptyString) {
    llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, "");
    emptyString = new GlobalVariable(*context.getModule(),
                                     stringConstant->getType(),
                                     true,
                                     GlobalValue::InternalLinkage,
                                     stringConstant,
                                     EMPTY_STRING_CONSTANT_NAME);
  }

  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = emptyString->getType()->getPointerElementType();
  
  Value* value = ConstantExpr::getGetElementPtr(elementType, emptyString, Idx);

  IRWriter::newStoreInst(context, value, alloc);
}

void StringType::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object);
  context.getScopes().setVariable(variable);
}

void StringType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* StringType::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const PrimitiveTypeSpecifier* StringType::newTypeSpecifier() const {
  return new PrimitiveTypeSpecifier(this);
}

Instruction* StringType::inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
