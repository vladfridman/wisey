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
  wisey::ArrayType* arrayType = mArrayTypeSpecifier->getType(context);
  const IType* elementType = arrayType->getElementType();
  if (!elementType->isImmutable() && !elementType->isPrimitive()) {
    context.reportError(mArrayTypeSpecifier->getLine(),
                        "Immutable array base type can only be of primitive or immutable type");
    exit(1);
  }
  return arrayType->getImmutable();
}

void ImmutableArrayTypeSpecifier::printToStream(IRGenerationContext& context,
                                                iostream& stream) const {
  stream << "immutable ";
  mArrayTypeSpecifier->printToStream(context, stream);
}

int ImmutableArrayTypeSpecifier::getLine() const {
  return mArrayTypeSpecifier->getLine();
}
