//
//  Block.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Block.hpp"
#include "yazyk/Scope.hpp"

using namespace llvm;
using namespace yazyk;

Value* Block::generateIR(IRGenerationContext& context) const {
  context.pushScope();
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = mStatements.begin(); it != mStatements.end(); it++) {
    IStatement *statement = *it;
    last = statement->generateIR(context);
  }
  context.popScope();
  return NULL;
}

StatementList& Block::getStatements() {
  return mStatements;
}
