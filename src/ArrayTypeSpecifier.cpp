//
//  ArrayTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

ArrayTypeSpecifier::ArrayTypeSpecifier(ITypeSpecifier* elementTypeSpecifier,
                                       std::vector<unsigned long> dimensions) :
mElementTypeSpecifier(elementTypeSpecifier), mDimensions(dimensions) {
}

ArrayTypeSpecifier::~ArrayTypeSpecifier() {
  delete mElementTypeSpecifier;
}

ArrayType* ArrayTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getArrayType(mElementTypeSpecifier->getType(context), mDimensions);
}

void ArrayTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  mElementTypeSpecifier->printToStream(context, stream);
  for (long dimension : mDimensions) {
    stream << "[" << dimension << "]";
  }
}
