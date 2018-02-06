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
                                                       std::list<const IExpression*> dimensions) :
mElementTypeSpecifier(elementTypeSpecifier), mDimensions(dimensions) {
}

ArraySpecificTypeSpecifier::~ArraySpecificTypeSpecifier() {
  delete mElementTypeSpecifier;
  for (const IExpression* expression : mDimensions) {
    delete expression;
  }
}

ArraySpecificType* ArraySpecificTypeSpecifier::getType(IRGenerationContext& context) const {
  return new ArraySpecificType(mElementTypeSpecifier->getType(context), mDimensions);
}

void ArraySpecificTypeSpecifier::printToStream(IRGenerationContext& context,
                                               iostream &stream) const {
  mElementTypeSpecifier->printToStream(context, stream);
  for (const IExpression* dimension : mDimensions) {
    stream << "[";
    dimension->printToStream(context, stream);
    stream << "]";
  }
}

