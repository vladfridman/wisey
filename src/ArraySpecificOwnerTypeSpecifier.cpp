//
//  ArraySpecificOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ArraySpecificOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ArraySpecificOwnerTypeSpecifier::ArraySpecificOwnerTypeSpecifier(ArraySpecificTypeSpecifier*
                                                                 arraySpecificTypeSpecifier) :
mArraySpecificTypeSpecifier(arraySpecificTypeSpecifier) {
}

ArraySpecificOwnerTypeSpecifier::~ArraySpecificOwnerTypeSpecifier() {
  delete mArraySpecificTypeSpecifier;
}

const ArraySpecificOwnerType* ArraySpecificOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mArraySpecificTypeSpecifier->getType(context)->getOwner();
}

void ArraySpecificOwnerTypeSpecifier::printToStream(IRGenerationContext &context, iostream &stream) const {
  mArraySpecificTypeSpecifier->printToStream(context, stream);
  stream << "*";
}
