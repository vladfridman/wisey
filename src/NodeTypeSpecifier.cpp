//
//  NodeTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Node.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node* NodeTypeSpecifier::getType(IRGenerationContext& context) const {
  string fullName = "";
  for (string part : mPackage) {
    fullName.append(part + ".");
  }
  fullName.append(mName);
  
  return context.getNode(fullName);
}

void NodeTypeSpecifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  printPackageToStream(stream, mPackage);
  stream << mName;
}
