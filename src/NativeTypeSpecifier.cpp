//
//  NativeTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/Log.hpp"
#include "wisey/NativeTypeSpecifier.hpp"

using namespace llvm;
using namespace wisey;

NativeTypeSpecifier::NativeTypeSpecifier(IType* type) : mType(type) {
}

NativeTypeSpecifier::~NativeTypeSpecifier() {
  delete mType;
}

IType* NativeTypeSpecifier::getType(IRGenerationContext& context) const {
  return mType;
}

void NativeTypeSpecifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  mType->printToStream(context, stream);
}

