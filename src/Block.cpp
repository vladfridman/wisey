//
//  Block.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "Block.hpp"
#include "IRGenerationContext.hpp"
#include "Scope.hpp"

using namespace llvm;
using namespace wisey;

Block::Block() { }

Block::~Block() {
  for (IStatement* statement : mStatements) {
    delete statement;
  }
  mStatements.clear();
}

void Block::generateIR(IRGenerationContext& context) const {
  for (IStatement* statement : mStatements) {
    statement->generateIR(context);
  }
}

StatementList& Block::getStatements() {
  return mStatements;
}
