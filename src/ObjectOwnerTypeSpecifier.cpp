//
//  ObjectOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ObjectOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ObjectOwnerTypeSpecifier::ObjectOwnerTypeSpecifier(const IObjectTypeSpecifier* objectTypeSpecifier) :
mObjectTypeSpecifier(objectTypeSpecifier) { }

ObjectOwnerTypeSpecifier::~ObjectOwnerTypeSpecifier() {
  delete mObjectTypeSpecifier;
}

const IObjectOwnerType* ObjectOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mObjectTypeSpecifier->getType(context)->getOwner();
}

void ObjectOwnerTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "*";
}
