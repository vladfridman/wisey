//
//  CompoundStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/CompoundStatement.hpp"

using namespace llvm;
using namespace yazyk;

Value* CompoundStatement::generateIR(IRGenerationContext& context) const {
  context.pushScope();
  mBlock.generateIR(context);
  context.popScope();
  
  return NULL;
}
