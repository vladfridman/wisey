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

std::string InterfaceTypeSpecifier::getShortName() const {
  return mShortName;
}

std::string InterfaceTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackage);
}

Interface* InterfaceTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getInterface(getName(context));
}

void InterfaceTypeSpecifier::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  stream << getName(context);
}
