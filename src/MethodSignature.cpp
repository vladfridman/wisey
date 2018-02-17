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

ObjectElementType MethodSignature::getObjectElementType() const {
  return OBJECT_ELEMENT_METHOD_SIGNATURE;
}

string MethodSignature::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

FunctionType* MethodSignature::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
}

TypeKind MethodSignature::getTypeKind() const {
  return TypeKind::FUNCTION_TYPE;
}

bool MethodSignature::canCastTo(const IType* toType) const {
  return false;
}

bool MethodSignature::canAutoCastTo(const IType *toType) const {
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

const IObjectType* MethodSignature::getObjectType() const {
  return mObjectType;
}

void MethodSignature::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void MethodSignature::allocateVariable(IRGenerationContext& context, string name) const {
  assert(false);
}
