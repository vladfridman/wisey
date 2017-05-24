//
//  CaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CaseStatement.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

void CaseStatement::prototype(IRGenerationContext& context) const {
}

Value* CaseStatement::generateIR(IRGenerationContext& context) const {
  return mBlock.generateIR(context);
}

bool CaseStatement::isFallThrough() const {
  return mIsFallThrough;
}

ConstantInt* CaseStatement::getExpressionValue(IRGenerationContext& context) const {
  Value* value = mExpression.generateIR(context);
  if (!ConstantInt::classof(value)) {
    Log::e("Case expression should be an integer constant");
    exit(1);
  }
  
  return (ConstantInt*) value;
}
