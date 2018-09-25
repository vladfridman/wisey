//
//  PrimitiveTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "Log.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace wisey;

PrimitiveTypeSpecifier::PrimitiveTypeSpecifier(const IType* type, int line) :
mType(type), mLine(line) {
}

PrimitiveTypeSpecifier::~PrimitiveTypeSpecifier() {
}

const IType* PrimitiveTypeSpecifier::getType(IRGenerationContext& context) const {
  return mType;
}

void PrimitiveTypeSpecifier::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  stream << mType->getTypeName();
}

int PrimitiveTypeSpecifier::getLine() const {
  return mLine;
}
