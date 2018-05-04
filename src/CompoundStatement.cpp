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

CompoundStatement::CompoundStatement(Block* block, int line) : mBlock(block), mLine(line) { }

CompoundStatement::~CompoundStatement() {
  delete mBlock;
}

void CompoundStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);

  Scopes& scopes = context.getScopes();
  
  scopes.pushScope();
  mBlock->generateIR(context);
  scopes.popScope(context, mLine);
}
