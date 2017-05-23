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

IType* InterfaceTypeSpecifier::getType(IRGenerationContext& context) const {
  if (!mPackage.size()) {
    IObjectType* object = context.getImport(mName);
    return context.getInterface(object->getName());
  }
  
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);

  return context.getInterface(fullName);
}
