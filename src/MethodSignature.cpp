//
//  MethodSignature.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
  return mAccessLevel;
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

MethodSignature* MethodSignature::createCopy() const {
  return new MethodSignature(mName, mAccessLevel, mReturnType, mArguments, mThrownExceptions);
}

void MethodSignature::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}
