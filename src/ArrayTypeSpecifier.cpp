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

ArrayTypeSpecifier::ArrayTypeSpecifier(ITypeSpecifier* baseTypeSpecifier, unsigned long arraySize) :
mBaseTypeSpecifier(baseTypeSpecifier), mSize(arraySize) {
}

ArrayTypeSpecifier::~ArrayTypeSpecifier() {
  delete mBaseTypeSpecifier;
}

ArrayType* ArrayTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getArrayType(mBaseTypeSpecifier->getType(context), mSize);
}

void ArrayTypeSpecifier::printToStream(IRGenerationContext &context, iostream &stream) const {
  mBaseTypeSpecifier->printToStream(context, stream);
  stream << "[" << mSize << "]";
}
