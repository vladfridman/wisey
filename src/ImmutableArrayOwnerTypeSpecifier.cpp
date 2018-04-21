//
//  ImmutableArrayOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ImmutableArrayOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ImmutableArrayOwnerTypeSpecifier::
ImmutableArrayOwnerTypeSpecifier(ImmutableArrayTypeSpecifier* immutableArrayTypeSpecifier) :
mImmutableArrayTypeSpecifier(immutableArrayTypeSpecifier) {
}

ImmutableArrayOwnerTypeSpecifier::~ImmutableArrayOwnerTypeSpecifier() {
  delete mImmutableArrayTypeSpecifier;
}

const ImmutableArrayOwnerType* ImmutableArrayOwnerTypeSpecifier::
getType(IRGenerationContext& context) const {
  return mImmutableArrayTypeSpecifier->getType(context)->getOwner();
}

void ImmutableArrayOwnerTypeSpecifier::printToStream(IRGenerationContext &context,
                                                     iostream &stream) const {
  mImmutableArrayTypeSpecifier->printToStream(context, stream);
  stream << "*";
}

int ImmutableArrayOwnerTypeSpecifier::getLine() const {
  return mImmutableArrayTypeSpecifier->getLine();
}
