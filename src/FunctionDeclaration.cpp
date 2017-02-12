//
//  FunctionDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/FunctionDeclaration.hpp"

using namespace llvm;
using namespace yazyk;

Value* FunctionDeclaration::generateIR(IRGenerationContext& context) const {
  return mMethodDeclaration->generateIR(context);
}
