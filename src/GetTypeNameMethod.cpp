//
//  GetTypeNameMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "GetTypeNameMethod.hpp"
#include "IExpression.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

GetTypeNameMethod::GetTypeNameMethod() : mMethodQualifiers(new MethodQualifiers(0)) {
}

GetTypeNameMethod::~GetTypeNameMethod() {
  delete mMethodQualifiers;
}

const GetTypeNameMethod* GetTypeNameMethod::GET_TYPE_NAME_METHOD = new GetTypeNameMethod();

const std::string GetTypeNameMethod::GET_TYPE_NAME_METHOD_NAME = "getTypeName";

Value* GetTypeNameMethod::generateIR(IRGenerationContext& context,
                                     const IObjectType* objectType) {
  return IObjectType::getObjectNamePointer(objectType, context);
}

string GetTypeNameMethod::getName() const {
  return GET_TYPE_NAME_METHOD_NAME;
}

bool GetTypeNameMethod::isPublic() const {
  return true;
}

const IType* GetTypeNameMethod::getReturnType() const {
  return PrimitiveTypes::STRING;
}

vector<const wisey::Argument*> GetTypeNameMethod::getArguments() const {
  vector<const wisey::Argument*> emptyList;
  return emptyList;
}

vector<const Model*> GetTypeNameMethod::getThrownExceptions() const {
  vector<const Model*> emptyList;
  return emptyList;
}

bool GetTypeNameMethod::isStatic() const {
  return true;
}

bool GetTypeNameMethod::isOverride() const {
  return false;
}

FunctionType* GetTypeNameMethod::getLLVMType(IRGenerationContext& context) const {
  return FunctionType::get(PrimitiveTypes::STRING->getLLVMType(context), false);
}

const IObjectType* GetTypeNameMethod::getParentObject() const {
  return NULL;
}

MethodQualifiers* GetTypeNameMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string GetTypeNameMethod::getTypeName() const {
  return GET_TYPE_NAME_METHOD_NAME;
}

bool GetTypeNameMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool GetTypeNameMethod::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

Value* GetTypeNameMethod::castTo(IRGenerationContext& context,
                                 Value* fromValue,
                                 const IType* toType,
                                 int line) const {
  assert(false && "Attempting to cast array getSize() method");
}

bool GetTypeNameMethod::isPrimitive() const {
  return false;
}

bool GetTypeNameMethod::isOwner() const {
  return false;
}

bool GetTypeNameMethod::isReference() const {
  return false;
}

bool GetTypeNameMethod::isArray() const {
  return false;
}

bool GetTypeNameMethod::isFunction() const {
  return true;
}

bool GetTypeNameMethod::isPackage() const {
  return false;
}

bool GetTypeNameMethod::isController() const {
  return false;
}

bool GetTypeNameMethod::isInterface() const {
  return false;
}

bool GetTypeNameMethod::isModel() const {
  return false;
}

bool GetTypeNameMethod::isNode() const {
  return false;
}

bool GetTypeNameMethod::isNative() const {
  return false;
}

bool GetTypeNameMethod::isPointer() const {
  return false;
}

bool GetTypeNameMethod::isImmutable() const {
  return false;
}

void GetTypeNameMethod::createLocalVariable(IRGenerationContext& context,
                                            std::string name,
                                            int line) const {
  assert(false && "Tring to create a variable of method type");
}

void GetTypeNameMethod::createFieldVariable(IRGenerationContext& context,
                                            std::string name,
                                            const IConcreteObjectType* object,
                                            int line) const {
  assert(false && "Tring to create a variable of method type");
}

void GetTypeNameMethod::createParameterVariable(IRGenerationContext& context,
                                                std::string name,
                                                llvm::Value* value,
                                                int line) const {
  assert(false && "Tring to create a variable of method type");
}

const wisey::ArrayType* GetTypeNameMethod::getArrayType(IRGenerationContext& context,
                                                        int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Value* GetTypeNameMethod::inject(IRGenerationContext& context,
                                 const InjectionArgumentList injectionArgumentList,
                                 int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

void GetTypeNameMethod:: printToStream(IRGenerationContext& context,
                                       iostream& stream) const {
  assert(false && "Attempt to print array getSize() method");
}
