//
//  Block.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Block.hpp"

using namespace llvm;
using namespace yazyk;

Value* Block::generateIR(IRGenerationContext& context) {
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    IStatement *statement = *it;
    last = statement->generateIR(context);
  }
  return last;
}

StatementList& Block::getStatements() {
  return statements;
}
