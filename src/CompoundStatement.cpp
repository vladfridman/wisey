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
  Scopes& scopes = context.getScopes();
  
  scopes.pushScope();
  mBlock.generateIR(context);
  scopes.popScope(context.getBasicBlock());
  
  return NULL;
}
