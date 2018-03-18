//
//  LLVMPrimitiveTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace wisey;

const ILLVMPrimitiveType* LLVMPrimitiveTypeSpecifier::getType(IRGenerationContext& context) const {
  return mType;
}

void LLVMPrimitiveTypeSpecifier::printToStream(IRGenerationContext& context,
                                               std::iostream& stream) const {
  stream << mType->getTypeName();
}
