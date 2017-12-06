//
//  ExternalStaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalStaticMethod::ExternalStaticMethod(string name,
                                           const IType* returnType,
                                           vector<MethodArgument*> arguments,
                                           vector<const Model*> thrownExceptions) :
mName(name),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

ExternalStaticMethod::~ExternalStaticMethod() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool ExternalStaticMethod::isStatic() const {
  return true;
}

Function* ExternalStaticMethod::defineFunction(IRGenerationContext& context,
                                               const IObjectType* object) {
  return IMethod::defineFunction(context, object, this);
}

void ExternalStaticMethod::generateIR(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
}

string ExternalStaticMethod::getName() const {
  return mName;
}

AccessLevel ExternalStaticMethod::getAccessLevel() const {
  return AccessLevel::PUBLIC_ACCESS;
}

const IType* ExternalStaticMethod::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> ExternalStaticMethod::getArguments() const {
  return mArguments;
}

vector<const Model*> ExternalStaticMethod::getThrownExceptions() const {
  return mThrownExceptions;
}

ObjectElementType ExternalStaticMethod::getObjectElementType() const {
  return OBJECT_ELEMENT_STATIC_METHOD;
}

void ExternalStaticMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
}

