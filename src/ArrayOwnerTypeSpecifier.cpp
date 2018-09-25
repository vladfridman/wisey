//
//  ArrayOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ArrayOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ArrayOwnerTypeSpecifier::ArrayOwnerTypeSpecifier(ArrayTypeSpecifier* arrayTypeSpecifier) :
mArrayTypeSpecifier(arrayTypeSpecifier) {
}

ArrayOwnerTypeSpecifier::~ArrayOwnerTypeSpecifier() {
  delete mArrayTypeSpecifier;
}

const ArrayOwnerType* ArrayOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mArrayTypeSpecifier->getType(context)->getOwner();
}

void ArrayOwnerTypeSpecifier::printToStream(IRGenerationContext &context, iostream &stream) const {
  mArrayTypeSpecifier->printToStream(context, stream);
  stream << "*";
}

int ArrayOwnerTypeSpecifier::getLine() const {
  return mArrayTypeSpecifier->getLine();
}
