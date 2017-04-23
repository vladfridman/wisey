//
//  MethodSignature.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string MethodSignature::getName() const {
  return mName;
}

AccessLevel MethodSignature::getAccessLevel() const {
  return mAccessLevel;
}

IType* MethodSignature::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> MethodSignature::getArguments() const {
  return mArguments;
}

unsigned long MethodSignature::getIndex() const {
  return mIndex;
}

vector<IType*> MethodSignature::getThrownExceptions() const {
  return mThrownExceptions;
}

MethodSignature* MethodSignature::createCopyWithIndex(unsigned long index) const {
  return new MethodSignature(mName,
                             mAccessLevel,
                             mReturnType,
                             mArguments,
                             mThrownExceptions,
                             index);
}
