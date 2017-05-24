//
//  Block.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Block.hpp"
#include "wisey/Scope.hpp"

using namespace llvm;
using namespace wisey;

void Block::prototype(IRGenerationContext& context) const {
  for (IStatement* statement : mStatements) {
    statement->prototype(context);
  }
}

Value* Block::generateIR(IRGenerationContext& context) const {
  for (IStatement* statement : mStatements) {
    statement->generateIR(context);
  }
  return NULL;
}

StatementList& Block::getStatements() {
  return mStatements;
}
