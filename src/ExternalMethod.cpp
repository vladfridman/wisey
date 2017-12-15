//
//  ExternalMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalMethod::ExternalMethod(const IObjectType* objectType,
                               string name,
                               const IType* returnType,
                               vector<MethodArgument*> arguments,
                               vector<const Model*> thrownExceptions) :
mObjectType(objectType),
mName(name),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

ExternalMethod::~ExternalMethod() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool ExternalMethod::isStatic() const {
  return false;
}

Function* ExternalMethod::defineFunction(IRGenerationContext& context) {
  return IMethod::defineFunction(context, mObjectType, this);
}

void ExternalMethod::generateIR(IRGenerationContext& context) const {
}

string ExternalMethod::getName() const {
  return mName;
}

AccessLevel ExternalMethod::getAccessLevel() const {
  return AccessLevel::PUBLIC_ACCESS;
}

const IType* ExternalMethod::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> ExternalMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> ExternalMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

ObjectElementType ExternalMethod::getObjectElementType() const {
  return OBJECT_ELEMENT_METHOD;
}

string ExternalMethod::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* ExternalMethod::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
}

TypeKind ExternalMethod::getTypeKind() const {
  return TypeKind::FUNCTION_TYPE;
}

bool ExternalMethod::canCastTo(const IType* toType) const {
  return false;
}

bool ExternalMethod::canAutoCastTo(const IType *toType) const {
  return false;
}

Value* ExternalMethod::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  return NULL;
}

const IObjectType* ExternalMethod::getObjectType() const {
  return mObjectType;
}

void ExternalMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
}
