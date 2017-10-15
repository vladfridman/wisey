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

std::string ControllerTypeSpecifier::getShortName() const {
  return mShortName;
}

std::string ControllerTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackage);
}

const Controller* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getController(getName(context));
}

void ControllerTypeSpecifier::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  stream << getName(context);
}
