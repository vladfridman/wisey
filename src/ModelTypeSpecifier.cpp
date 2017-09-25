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

Model* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);

  return context.getModel(fullName);
}

void ModelTypeSpecifier::printToStream(std::iostream& stream) const {
  printPackageToStream(stream, mPackage);
  stream << mName;
}
