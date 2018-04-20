//
//  InterfaceTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceTypeSpecifier::InterfaceTypeSpecifier(IExpression* packageExpression,
                                               string shortName,
                                               int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

InterfaceTypeSpecifier::~InterfaceTypeSpecifier() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string InterfaceTypeSpecifier::getShortName() const {
  return mShortName;
}

IExpression* InterfaceTypeSpecifier::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string InterfaceTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackageExpression);
}

Interface* InterfaceTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getInterface(getName(context), mLine);
}

void InterfaceTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int InterfaceTypeSpecifier::getLine() const {
  return mLine;
}
