//
//  GetObjectNameMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/GetObjectNameMethod.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

GetObjectNameMethod::GetObjectNameMethod() : mMethodQualifiers(new MethodQualifiers(0)) {
}

GetObjectNameMethod::~GetObjectNameMethod() {
  delete mMethodQualifiers;
}

const GetObjectNameMethod* GetObjectNameMethod::GET_OBJECT_NAME_METHOD = new GetObjectNameMethod();

const std::string GetObjectNameMethod::GET_OBJECT_NAME_METHOD_NAME = "getObjectName";

Value* GetObjectNameMethod::generateIR(IRGenerationContext& context,
                                       const IExpression* expression) {
  Value* expressionValue = expression->generateIR(context, PrimitiveTypes::VOID);
  return GetOriginalObjectNameFunction::call(context, expressionValue);
}

string GetObjectNameMethod::getName() const {
  return GET_OBJECT_NAME_METHOD_NAME;
}

bool GetObjectNameMethod::isPublic() const {
  return true;
}

const IType* GetObjectNameMethod::getReturnType() const {
  return PrimitiveTypes::STRING;
}

vector<const wisey::Argument*> GetObjectNameMethod::getArguments() const {
  vector<const wisey::Argument*> emptyList;
  return emptyList;
}

vector<const Model*> GetObjectNameMethod::getThrownExceptions() const {
  vector<const Model*> emptyList;
  return emptyList;
}

bool GetObjectNameMethod::isStatic() const {
  return false;
}

bool GetObjectNameMethod::isOverride() const {
  return false;
}

FunctionType* GetObjectNameMethod::getLLVMType(IRGenerationContext& context) const {
  return FunctionType::get(PrimitiveTypes::STRING->getLLVMType(context), false);
}

const IObjectType* GetObjectNameMethod::getParentObject() const {
  return NULL;
}

MethodQualifiers* GetObjectNameMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string GetObjectNameMethod::getTypeName() const {
  return "<object>." + GET_OBJECT_NAME_METHOD_NAME;
}

bool GetObjectNameMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool GetObjectNameMethod::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

Value* GetObjectNameMethod::castTo(IRGenerationContext& context,
                                   Value* fromValue,
                                   const IType* toType,
                                   int line) const {
  assert(false && "Attempting to cast array getSize() method");
}

bool GetObjectNameMethod::isPrimitive() const {
  return false;
}

bool GetObjectNameMethod::isOwner() const {
  return false;
}

bool GetObjectNameMethod::isReference() const {
  return false;
}

bool GetObjectNameMethod::isArray() const {
  return false;
}

bool GetObjectNameMethod::isFunction() const {
  return true;
}

bool GetObjectNameMethod::isPackage() const {
  return false;
}

bool GetObjectNameMethod::isController() const {
  return false;
}

bool GetObjectNameMethod::isInterface() const {
  return false;
}

bool GetObjectNameMethod::isModel() const {
  return false;
}

bool GetObjectNameMethod::isNode() const {
  return false;
}

bool GetObjectNameMethod::isNative() const {
  return false;
}

bool GetObjectNameMethod::isPointer() const {
  return false;
}

bool GetObjectNameMethod::isImmutable() const {
  return false;
}

void GetObjectNameMethod::createLocalVariable(IRGenerationContext& context,
                                              std::string name,
                                              int line) const {
  assert(false && "Tring to create a variable of method type");
}

void GetObjectNameMethod::createFieldVariable(IRGenerationContext& context,
                                              std::string name,
                                              const IConcreteObjectType* object,
                                              int line) const {
  assert(false && "Tring to create a variable of method type");
}

void GetObjectNameMethod::createParameterVariable(IRGenerationContext& context,
                                                  std::string name,
                                                  llvm::Value* value,
                                                  int line) const {
  assert(false && "Tring to create a variable of method type");
}

const wisey::ArrayType* GetObjectNameMethod::getArrayType(IRGenerationContext& context,
                                                          int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Value* GetObjectNameMethod::inject(IRGenerationContext& context,
                                   const InjectionArgumentList injectionArgumentList,
                                   int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

void GetObjectNameMethod:: printToStream(IRGenerationContext& context,
                                         iostream& stream) const {
  assert(false && "Attempt to print array getSize() method");
}

