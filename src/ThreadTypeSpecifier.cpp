//
//  ThreadTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ThreadTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ThreadTypeSpecifier::ThreadTypeSpecifier(IExpression* packageExpression, string shortName) :
mPackageExpression(packageExpression),
mShortName(shortName) {
}

ThreadTypeSpecifier::~ThreadTypeSpecifier() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ThreadTypeSpecifier::getShortName() const {
  return mShortName;
}

IExpression* ThreadTypeSpecifier::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ThreadTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackageExpression);
}

Thread* ThreadTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getThread(getName(context));
}

void ThreadTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

