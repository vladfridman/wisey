//
//  StringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "Cast.hpp"
#include "FakeExpression.hpp"
#include "FieldPrimitiveVariable.hpp"
#include "IObjectType.hpp"
#include "IRWriter.hpp"
#include "IdentifierChain.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "MethodCall.hpp"
#include "Names.hpp"
#include "ParameterPrimitiveVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "StringType.hpp"

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

llvm::PointerType* StringType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

bool StringType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isPointer() && toType->isNative()) {
    return true;
  }
  return toType == PrimitiveTypes::BOOLEAN || toType == PrimitiveTypes::STRING;
}

bool StringType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* StringType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType->isPointer() && toType->isNative()) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  }
  if (toType == PrimitiveTypes::STRING) {
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

bool StringType::isNative() const {
  return false;
}

bool StringType::isPointer() const {
  return false;
}

bool StringType::isImmutable() const {
  return false;
}

void StringType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void StringType::createLocalVariable(IRGenerationContext& context,
                                     string name,
                                     int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
  
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
                                     const IConcreteObjectType* object,
                                     int line) const {
  IVariable* variable = new FieldPrimitiveVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void StringType::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value,
                                         int line) const {
  IVariable* variable = new ParameterPrimitiveVariable(name, this, value, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* StringType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const PrimitiveTypeSpecifier* StringType::newTypeSpecifier(int line) const {
  return new PrimitiveTypeSpecifier(this, line);
}

Instruction* StringType::inject(IRGenerationContext& context,
                                const InjectionArgumentList injectionArgumentList,
                                int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

bool StringType::isStringVariation(IRGenerationContext& context, const IType* type, int line) {
  return type == PrimitiveTypes::STRING ||
  type == PrimitiveTypes::STRING_FORMAT ||
  isCharArray(context, type, line) ||
  !type->getTypeName().compare("wisey.lang.MString") ||
  !type->getTypeName().compare("wisey.lang.MString*") ||
  !type->getTypeName().compare("wisey.lang.NString") ||
  !type->getTypeName().compare("wisey.lang.NString*");
}

bool StringType::isCharArray(IRGenerationContext& context, const IType* type, int line) {
  return type->isArray() &&
  type->getArrayType(context, line)->getNumberOfDimensions() == 1 &&
  type->getArrayType(context, line)->getElementType() == PrimitiveTypes::CHAR;
}

Value* StringType::callGetContent(IRGenerationContext& context,
                                  const IType* type,
                                  llvm::Value* object,
                                  int line) {
  assert(IType::isObjectType(type) && "Expecting wisey.lang.MString or wisey.lang.CString");
  
  IdentifierChain* identifierChain = new IdentifierChain(new FakeExpression(object, type),
                                                         "getContent",
                                                         line);
  ExpressionList arguments;
  MethodCall* methodCall = MethodCall::create(identifierChain, arguments, line);
  Value* result = methodCall->generateIR(context, PrimitiveTypes::VOID);
  delete methodCall;

  return result;
}
