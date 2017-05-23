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

IType* ControllerTypeSpecifier::getType(IRGenerationContext& context) const {
  if (!mPackage.size()) {
    IObjectType* object = context.getImport(mName);
    return context.getController(object->getFullName());
  }
  
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);

  return context.getController(fullName);
}

