//
//  MethodSignature.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Argument.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Interface.hpp"
#include "wisey/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodSignature::MethodSignature(const IObjectType* objectType,
                                 std::string name,
                                 const IType* returnType,
                                 vector<const Argument*> arguments,
                                 vector<const Model*> thrownExceptions,
                                 MethodQualifiers* methodQualifiers,
                                 string originalParentName,
                                 int line) :
mObjectType(objectType),
mName(name),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(methodQualifiers),
mOriginalParentName(originalParentName),
mLine(line) { }

MethodSignature::~MethodSignature() {
  for (const Argument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  delete mMethodQualifiers;
}

bool MethodSignature::isStatic() const {
  return false;
}

string MethodSignature::getName() const {
  return mName;
}

bool MethodSignature::isPublic() const {
  return true;
}

const IType* MethodSignature::getReturnType() const {
  return mReturnType;
}

vector<const wisey::Argument*> MethodSignature::getArguments() const {
  return mArguments;
}

vector<const Model*> MethodSignature::getThrownExceptions() const {
  return mThrownExceptions;
}

MethodSignature* MethodSignature::createCopy(const Interface* interface) const {
  MethodQualifiers* methodQualifiers = new MethodQualifiers(mLine);
  for (MethodQualifier methodQualifier : mMethodQualifiers->getMethodQualifierSet()) {
    methodQualifiers->getMethodQualifierSet().insert(methodQualifier);
  }
  if (!methodQualifiers->getMethodQualifierSet().count(MethodQualifier::OVERRIDE)) {
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
  }
  return new MethodSignature(interface,
                             mName,
                             mReturnType,
                             mArguments,
                             mThrownExceptions,
                             methodQualifiers,
                             mOriginalParentName,
                             mLine);
}

string MethodSignature::getOriginalParentName() const {
  return mOriginalParentName;
}

int MethodSignature::getLine() const {
  return mLine;
}

bool MethodSignature::isConstant() const {
  return false;
}

bool MethodSignature::isField() const {
  return false;
}

bool MethodSignature::isMethod() const {
  return false;
}

bool MethodSignature::isStaticMethod() const {
  return false;
}

bool MethodSignature::isMethodSignature() const {
  return true;
}

bool MethodSignature::isLLVMFunction() const {
  return false;
}

bool MethodSignature::isOverride() const {
  return mMethodQualifiers->getMethodQualifierSet().count(MethodQualifier::OVERRIDE);
}

MethodQualifiers* MethodSignature::getMethodQualifiers() const {
  return mMethodQualifiers;
}

string MethodSignature::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* MethodSignature::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType, mLine);
}

bool MethodSignature::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool MethodSignature::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* MethodSignature::castTo(IRGenerationContext& context,
                               Value* fromValue,
                               const IType* toType,
                               int line) const {
  return NULL;
}

bool MethodSignature::isOwner() const {
  return false;
}

bool MethodSignature::isReference() const {
  return false;
}

bool MethodSignature::isArray() const {
  return false;
}

bool MethodSignature::isFunction() const {
  return true;
}

bool MethodSignature::isPackage() const {
  return false;
}

bool MethodSignature::isPrimitive() const {
  return false;
}

bool MethodSignature::isController() const {
  return false;
}

bool MethodSignature::isInterface() const {
  return false;
}

bool MethodSignature::isModel() const {
  return false;
}

bool MethodSignature::isNode() const {
  return false;
}

bool MethodSignature::isNative() const {
  return false;
}

bool MethodSignature::isPointer() const {
  return false;
}

bool MethodSignature::isImmutable() const {
  return false;
}

const IObjectType* MethodSignature::getParentObject() const {
  return mObjectType;
}

void MethodSignature::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void MethodSignature::createLocalVariable(IRGenerationContext& context,
                                          string name,
                                          int line) const {
  assert(false);
}

void MethodSignature::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object,
                                          int line) const {
  assert(false);
}

void MethodSignature::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value,
                                              int line) const {
  assert(false);
}

const wisey::ArrayType* MethodSignature::getArrayType(IRGenerationContext& context, int line) const {
  ArrayType::reportNonArrayType(context, line);
  throw 1;
}

Instruction* MethodSignature::inject(IRGenerationContext& context,
                                     const InjectionArgumentList injectionArgumentList,
                                     int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
