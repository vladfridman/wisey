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

AccessSpecifier MethodSignature::getAccessSpecifier() const {
  return mAccessSpecifier;
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

MethodSignature* MethodSignature::createCopyWithIndex(unsigned long index) const {
  return new MethodSignature(mName, mAccessSpecifier, mReturnType, mArguments, index);
}
