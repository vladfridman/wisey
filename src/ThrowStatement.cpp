//
//  ThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* ThrowStatement::generateIR(IRGenerationContext& context) const {
  if (mExpression.getType(context)->getTypeKind() != MODEL_TYPE) {
    Log::e("Thrown object can only be a model");
    exit(1);
  }
  
  return NULL;
}
