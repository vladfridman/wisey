//
//  ExternalMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/Argument.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalMethod::ExternalMethod(const IObjectType* objectType,
                               string name,
                               const IType* returnType,
                               vector<const Argument*> arguments,
                               vector<const Model*> thrownExceptions,
                               MethodQualifiers* methodQualifiers,
                               int line) :
mObjectType(objectType),
mName(name),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(methodQualifiers),
mLine(line) { }

ExternalMethod::~ExternalMethod() {
  for (const Argument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool ExternalMethod::isStatic() const {
  return false;
}

Function* ExternalMethod::defineFunction(IRGenerationContext& context) const {
  return IMethod::defineFunction(context, mObjectType, this);
}

void ExternalMethod::generateIR(IRGenerationContext& context) const {
}

string ExternalMethod::getName() const {
  return mName;
}

bool ExternalMethod::isPublic() const {
  return true;
}

const IType* ExternalMethod::getReturnType() const {
  return mReturnType;
}

vector<const wisey::Argument*> ExternalMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> ExternalMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

int ExternalMethod::getLine() const {
  return mLine;
}

bool ExternalMethod::isConstant() const {
  return false;
}

bool ExternalMethod::isField() const {
  return false;
}

bool ExternalMethod::isMethod() const {
  return true;
}

bool ExternalMethod::isStaticMethod() const {
  return false;
}

bool ExternalMethod::isMethodSignature() const {
  return false;
}

bool ExternalMethod::isLLVMFunction() const {
  return false;
}

bool ExternalMethod::isOverride() const {
  return mMethodQualifiers->getMethodQualifierSet().count(MethodQualifier::OVERRIDE);
}

MethodQualifiers* ExternalMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string ExternalMethod::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* ExternalMethod::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType, mLine);
}

bool ExternalMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool ExternalMethod::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* ExternalMethod::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  return NULL;
}

bool ExternalMethod::isPrimitive() const {
  return false;
}

bool ExternalMethod::isOwner() const {
  return false;
}

bool ExternalMethod::isReference() const {
  return false;
}

bool ExternalMethod::isArray() const {
  return false;
}

bool ExternalMethod::isFunction() const {
  return true;
}

bool ExternalMethod::isPackage() const {
  return false;
}

bool ExternalMethod::isController() const {
  return false;
}

bool ExternalMethod::isInterface() const {
  return false;
}

bool ExternalMethod::isModel() const {
  return false;
}

bool ExternalMethod::isNode() const {
  return false;
}

bool ExternalMethod::isNative() const {
  return false;
}

bool ExternalMethod::isPointer() const {
  return false;
}

bool ExternalMethod::isImmutable() const {
  return false;
}

const IObjectType* ExternalMethod::getParentObject() const {
  return mObjectType;
}

void ExternalMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
}

void ExternalMethod::createLocalVariable(IRGenerationContext& context,
                                         string name,
                                         int line) const {
  assert(false);
}

void ExternalMethod::createFieldVariable(IRGenerationContext& context,
                                         string name,
                                         const IConcreteObjectType* object,
                                         int line) const {
  assert(false);
}

void ExternalMethod::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value,
                                             int line) const {
  assert(false);
}

const wisey::ArrayType* ExternalMethod::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  throw 1;
}

Instruction* ExternalMethod::inject(IRGenerationContext& context,
                                    const InjectionArgumentList injectionArgumentList,
                                    int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
