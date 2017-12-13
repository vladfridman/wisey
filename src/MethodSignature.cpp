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

FunctionType* MethodSignature::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType);
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

void MethodSignature::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}
