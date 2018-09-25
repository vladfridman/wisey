//
//  ControllerTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ControllerTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerTypeSpecifierFull::ControllerTypeSpecifierFull(IExpression* packageExpression,
                                                         string shortName,
                                                         int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

ControllerTypeSpecifierFull::~ControllerTypeSpecifierFull() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ControllerTypeSpecifierFull::getShortName() const {
  return mShortName;
}

IExpression* ControllerTypeSpecifierFull::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ControllerTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackageExpression->getType(context)->getTypeName() + "." + mShortName;
}

const Controller* ControllerTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getController(getName(context), mLine);
}

void ControllerTypeSpecifierFull::printToStream(IRGenerationContext& context,
                                                iostream& stream) const {
  stream << getName(context);
}

int ControllerTypeSpecifierFull::getLine() const {
  return mLine;
}
