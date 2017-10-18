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

OwnerTypeSpecifier::OwnerTypeSpecifier(const IObjectTypeSpecifier* objectTypeSpecifier) :
mObjectTypeSpecifier(objectTypeSpecifier) { }

OwnerTypeSpecifier::~OwnerTypeSpecifier() {
  delete mObjectTypeSpecifier;
}

const IObjectOwnerType* OwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mObjectTypeSpecifier->getType(context)->getOwner();
}

void OwnerTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "*";
}
