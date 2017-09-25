//
//  OwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/OwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

OwnerTypeSpecifier::~OwnerTypeSpecifier() {
  delete mObjectTypeSpecifier;
}

const IObjectOwnerType* OwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mObjectTypeSpecifier->getType(context)->getOwner();
}

void OwnerTypeSpecifier::printToStream(std::iostream& stream) const {
  mObjectTypeSpecifier->printToStream(stream);
  stream << "*";
}
