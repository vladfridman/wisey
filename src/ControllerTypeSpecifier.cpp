//
//  ControllerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerTypeSpecifier::ControllerTypeSpecifier(IExpression* packageExpression,
                                                 string shortName,
                                                 int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

ControllerTypeSpecifier::~ControllerTypeSpecifier() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ControllerTypeSpecifier::getShortName() const {
  return mShortName;
}

IExpression* ControllerTypeSpecifier::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ControllerTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackageExpression, mLine);
}

const Controller* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getController(getName(context), mLine);
}

void ControllerTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int ControllerTypeSpecifier::getLine() const {
  return mLine;
}
