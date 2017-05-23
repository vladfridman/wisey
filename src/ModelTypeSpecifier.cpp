//
//  ModelTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IType* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  if (!mPackage.size()) {
    IObjectType* object = context.getImport(mName);
    return context.getModel(object->getFullName());
  }
  
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);

  return context.getModel(fullName);
}
