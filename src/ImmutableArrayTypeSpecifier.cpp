//
//  ImmutableArrayTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayType.hpp"
#include "wisey/ImmutableArrayTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ImmutableArrayTypeSpecifier::
ImmutableArrayTypeSpecifier(const ArrayTypeSpecifier* arrayTypeSpecifier) :
mArrayTypeSpecifier(arrayTypeSpecifier) {
}

ImmutableArrayTypeSpecifier::~ImmutableArrayTypeSpecifier() {
  delete mArrayTypeSpecifier;
}

const ImmutableArrayType* ImmutableArrayTypeSpecifier::getType(IRGenerationContext& context) const {
  return mArrayTypeSpecifier->getType(context)->getImmutable();
}

void ImmutableArrayTypeSpecifier::printToStream(IRGenerationContext& context,
                                                iostream& stream) const {
  stream << "immutable ";
  mArrayTypeSpecifier->printToStream(context, stream);
}

int ImmutableArrayTypeSpecifier::getLine() const {
  return mArrayTypeSpecifier->getLine();
}
