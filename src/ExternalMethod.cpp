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

ExternalMethod::~ExternalMethod() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

bool ExternalMethod::isStatic() const {
  return false;
}

Function* ExternalMethod::defineFunction(IRGenerationContext& context, const IObjectType* object) {
  return IMethod::defineFunction(context, object, this);
}

void ExternalMethod::generateIR(IRGenerationContext& context, const IObjectType* objectType) const {
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

void ExternalMethod::printToStream(IRGenerationContext& context, iostream& stream) const {
}
