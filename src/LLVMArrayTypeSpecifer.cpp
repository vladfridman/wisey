//
//  LLVMArrayTypeSpecifer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMArrayTypeSpecifer.hpp"

using namespace std;
using namespace wisey;

LLVMArrayTypeSpecifer::LLVMArrayTypeSpecifer(const ITypeSpecifier* elementTypeSpecifier,
                                             std::list<unsigned long> dimensions) :
mElementTypeSpecifier(elementTypeSpecifier), mDimensions(dimensions) {
}

LLVMArrayTypeSpecifer::~LLVMArrayTypeSpecifer() {
  delete mElementTypeSpecifier;
  mDimensions.clear();
}

LLVMArrayType* LLVMArrayTypeSpecifer::getType(IRGenerationContext& context) const {
  return context.getLLVMArrayType(mElementTypeSpecifier->getType(context), mDimensions);
}

void LLVMArrayTypeSpecifer::printToStream(IRGenerationContext& context, iostream &stream) const {
  stream << "::llvm::array(";
  mElementTypeSpecifier->printToStream(context, stream);
  for (long dimension : mDimensions) {
    stream << ", " << dimension;
  }
  stream << ")";
}
