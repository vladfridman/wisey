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
                                       unsigned long numberOfDimentions) :
mElementTypeSpecifier(elementTypeSpecifier), mNumberOfDimensions(numberOfDimentions) {
}

ArrayTypeSpecifier::~ArrayTypeSpecifier() {
  delete mElementTypeSpecifier;
}

ArrayType* ArrayTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getArrayType(mElementTypeSpecifier->getType(context), mNumberOfDimensions);
}

void ArrayTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  mElementTypeSpecifier->printToStream(context, stream);
  for (unsigned long i = 0; i < mNumberOfDimensions; i++) {
    stream << "[]";
  }
}
