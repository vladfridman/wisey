//
//  CompoundStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CompoundStatement.hpp"

using namespace llvm;
using namespace wisey;

void CompoundStatement::prototype(IRGenerationContext& context) const {
}

Value* CompoundStatement::generateIR(IRGenerationContext& context) const {
  Scopes& scopes = context.getScopes();
  
  scopes.pushScope();
  mBlock.generateIR(context);
  scopes.popScope(context);
  
  return NULL;
}
