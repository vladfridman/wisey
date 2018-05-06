//
//  ExternalStaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/Argument.hpp"
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalStaticMethod::ExternalStaticMethod(const IObjectType* objectType,
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

ExternalStaticMethod::~ExternalStaticMethod() {
  for (const Argument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool ExternalStaticMethod::isStatic() const {
  return true;
}

Function* ExternalStaticMethod::defineFunction(IRGenerationContext& context) const {
  return IMethod::defineFunction(context, mObjectType, this);
}

void ExternalStaticMethod::generateIR(IRGenerationContext& context) const {
}

string ExternalStaticMethod::getName() const {
  return mName;
}

bool ExternalStaticMethod::isPublic() const {
  return true;
}

const IType* ExternalStaticMethod::getReturnType() const {
  return mReturnType;
}

vector<const wisey::Argument*> ExternalStaticMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> ExternalStaticMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

int ExternalStaticMethod::getLine() const {
  return mLine;
}

bool ExternalStaticMethod::isConstant() const {
  return false;
}

bool ExternalStaticMethod::isField() const {
  return false;
}

bool ExternalStaticMethod::isMethod() const {
  return false;
}

bool ExternalStaticMethod::isStaticMethod() const {
  return true;
}

bool ExternalStaticMethod::isMethodSignature() const {
  return false;
}

bool ExternalStaticMethod::isLLVMFunction() const {
  return false;
}

bool ExternalStaticMethod::isOverride() const {
  return false;
}

MethodQualifiers* ExternalStaticMethod::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string ExternalStaticMethod::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* ExternalStaticMethod::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType, mLine);
}

bool ExternalStaticMethod::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool ExternalStaticMethod::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* ExternalStaticMethod::castTo(IRGenerationContext& context,
                                    Value* fromValue,
                                    const IType* toType,
                                    int line) const {
  return NULL;
}

bool ExternalStaticMethod::isOwner() const {
  return false;
}

bool ExternalStaticMethod::isReference() const {
  return false;
}

bool ExternalStaticMethod::isArray() const {
  return false;
}

bool ExternalStaticMethod::isFunction() const {
  return true;
}

bool ExternalStaticMethod::isPackage() const {
  return false;
}

bool ExternalStaticMethod::isPrimitive() const {
  return false;
}

bool ExternalStaticMethod::isController() const {
  return false;
}

bool ExternalStaticMethod::isInterface() const {
  return false;
}

bool ExternalStaticMethod::isModel() const {
  return false;
}

bool ExternalStaticMethod::isNode() const {
  return false;
}

bool ExternalStaticMethod::isNative() const {
  return false;
}

bool ExternalStaticMethod::isPointer() const {
  return false;
}

bool ExternalStaticMethod::isImmutable() const {
  return false;
}

const IObjectType* ExternalStaticMethod::getParentObject() const {
  return mObjectType;
}

void ExternalStaticMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
}

void ExternalStaticMethod::createLocalVariable(IRGenerationContext& context,
                                               string name,
                                               int line) const {
  assert(false);
}

void ExternalStaticMethod::createFieldVariable(IRGenerationContext& context,
                                               string name,
                                               const IConcreteObjectType* object,
                                               int line) const {
  assert(false);
}

void ExternalStaticMethod::createParameterVariable(IRGenerationContext& context,
                                                   string name,
                                                   Value* value,
                                                   int line) const {
  assert(false);
}

const wisey::ArrayType* ExternalStaticMethod::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* ExternalStaticMethod::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
