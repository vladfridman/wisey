//
//  ArraySpecificTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificType.hpp"
#include "wisey/ArraySpecificTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ArraySpecificTypeSpecifier::ArraySpecificTypeSpecifier(ITypeSpecifier* elementTypeSpecifier,
                                       std::vector<unsigned long> dimensions) :
mElementTypeSpecifier(elementTypeSpecifier), mDimensions(dimensions) {
}

ArraySpecificTypeSpecifier::~ArraySpecificTypeSpecifier() {
  delete mElementTypeSpecifier;
}

ArraySpecificType* ArraySpecificTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getArraySpecificType(mElementTypeSpecifier->getType(context), mDimensions);
}

void ArraySpecificTypeSpecifier::printToStream(IRGenerationContext& context,
                                               iostream &stream) const {
  mElementTypeSpecifier->printToStream(context, stream);
  for (long dimension : mDimensions) {
    stream << "[" << dimension << "]";
  }
}

