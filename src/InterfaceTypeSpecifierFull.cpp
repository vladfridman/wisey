//
//  InterfaceTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceTypeSpecifierFull::InterfaceTypeSpecifierFull(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

InterfaceTypeSpecifierFull::~InterfaceTypeSpecifierFull() { }

string InterfaceTypeSpecifierFull::getShortName() const {
  return mShortName;
}

string InterfaceTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackage + "." + mShortName;
}

Interface* InterfaceTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getInterface(getName(context));
}

void InterfaceTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

