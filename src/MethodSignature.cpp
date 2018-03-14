//
//  MethodSignature.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Interface.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodSignature::MethodSignature(const IObjectType* objectType,
                                 std::string name,
                                 const IType* returnType,
                                 vector<MethodArgument*> arguments,
                                 vector<const Model*> thrownExceptions) :
mObjectType(objectType),
mName(name),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

MethodSignature::~MethodSignature() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool MethodSignature::isStatic() const {
  return false;
}

string MethodSignature::getName() const {
  return mName;
}

AccessLevel MethodSignature::getAccessLevel() const {
  return AccessLevel::PUBLIC_ACCESS;
}

const IType* MethodSignature::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> MethodSignature::getArguments() const {
  return mArguments;
}

vector<const Model*> MethodSignature::getThrownExceptions() const {
  return mThrownExceptions;
}

MethodSignature* MethodSignature::createCopy(const Interface* interface) const {
  return new MethodSignature(interface, mName, mReturnType, mArguments, mThrownExceptions);
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

string MethodSignature::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* MethodSignature::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
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

bool MethodSignature::isThread() const {
  return false;
}

bool MethodSignature::isNative() const {
  return false;
}

const IObjectType* MethodSignature::getObjectType() const {
  return mObjectType;
}

void MethodSignature::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void MethodSignature::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void MethodSignature::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  assert(false);
}

void MethodSignature::createParameterVariable(IRGenerationContext& context,
                                              string name,
                                              Value* value) const {
  assert(false);
}

const wisey::ArrayType* MethodSignature::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const IType* MethodSignature::getPointerType() const {
  assert(false);
}

const IType* MethodSignature::getDereferenceType() const {
  assert(false);
}
