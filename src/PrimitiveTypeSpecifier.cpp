//
//  PrimitiveTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

Type* PrimitiveTypeSpecifier::getLLVMType(IRGenerationContext& context) const {
  return mType->getLLVMType(context.getLLVMContext());
}

VariableStorageType PrimitiveTypeSpecifier::getStorageType() const {
  return STACK_VARIABLE;
}
