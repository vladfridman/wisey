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

Value* Block::generateIR(IRGenerationContext& context) const {
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = mStatements.begin(); it != mStatements.end(); it++) {
    IStatement *statement = *it;
    last = statement->generateIR(context);
  }
  return NULL;
}

StatementList& Block::getStatements() {
  return mStatements;
}
