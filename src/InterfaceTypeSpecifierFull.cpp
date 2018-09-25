//
//  InterfaceTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "InterfaceTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceTypeSpecifierFull::InterfaceTypeSpecifierFull(IExpression* packageExpression,
                                                       string shortName,
                                                       int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

InterfaceTypeSpecifierFull::~InterfaceTypeSpecifierFull() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string InterfaceTypeSpecifierFull::getShortName() const {
  return mShortName;
}

IExpression* InterfaceTypeSpecifierFull::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string InterfaceTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackageExpression->getType(context)->getTypeName() + "." + mShortName;
}

Interface* InterfaceTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getInterface(getName(context), mLine);
}

void InterfaceTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int InterfaceTypeSpecifierFull::getLine() const {
  return mLine;
}
