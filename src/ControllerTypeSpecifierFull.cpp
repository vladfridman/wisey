//
//  ControllerTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerTypeSpecifierFull::ControllerTypeSpecifierFull(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

ControllerTypeSpecifierFull::~ControllerTypeSpecifierFull() { }

string ControllerTypeSpecifierFull::getShortName() const {
  return mShortName;
}

string ControllerTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackage + "." + mShortName;
}

const Controller* ControllerTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getController(getName(context));
}

void ControllerTypeSpecifierFull::printToStream(IRGenerationContext& context,
                                                iostream& stream) const {
  stream << getName(context);
}

