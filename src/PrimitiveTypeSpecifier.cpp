//
//  PrimitiveTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace wisey;

IType* PrimitiveTypeSpecifier::getType(IRGenerationContext& context) const {
  return mType;
}

void PrimitiveTypeSpecifier::printToStream(std::iostream& stream) const {
  stream << mType->getName();
}
