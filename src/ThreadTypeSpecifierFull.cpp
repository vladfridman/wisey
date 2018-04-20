//
//  ThreadTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ThreadTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ThreadTypeSpecifierFull::ThreadTypeSpecifierFull(IExpression* packageExpression,
                                                 string shortName,
                                                 int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

ThreadTypeSpecifierFull::~ThreadTypeSpecifierFull() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ThreadTypeSpecifierFull::getShortName() const {
  return mShortName;
}

IExpression* ThreadTypeSpecifierFull::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ThreadTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackageExpression->getType(context)->getTypeName() + "." + mShortName;
}

Thread* ThreadTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getThread(getName(context), mLine);
}

void ThreadTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int ThreadTypeSpecifierFull::getLine() const {
  return mLine;
}
