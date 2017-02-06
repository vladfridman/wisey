//
//  ModelTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

const string ModelTypeSpecifier::getName() const {
  return mName;
}

Type* ModelTypeSpecifier::getLLVMType(IRGenerationContext& context) const {
  return context.getModel(mName)->getLLVMType(context.getLLVMContext());
}

VariableStorageType ModelTypeSpecifier::getStorageType() const {
  return HEAP_VARIABLE;
}
