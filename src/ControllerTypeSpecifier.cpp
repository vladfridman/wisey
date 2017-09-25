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

Controller* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);

  return context.getController(fullName);
}

void ControllerTypeSpecifier::printToStream(std::iostream& stream) const {
  printPackageToStream(stream, mPackage);
  stream << mName;
}
