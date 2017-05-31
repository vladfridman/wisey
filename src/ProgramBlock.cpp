//
//  ProgramBlock.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramBlock.hpp"

using namespace llvm;
using namespace wisey;

void ProgramBlock::prototypeObjects(IRGenerationContext& context) const {
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeObjects(context);
  }
}

void ProgramBlock::prototypeMethods(IRGenerationContext& context) const {
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeMethods(context);
  }
}

Value* ProgramBlock::generateIR(IRGenerationContext& context) const {
  for (IStatement* statement : mStatements) {
    statement->generateIR(context);
  }
  return NULL;
}

StatementList& ProgramBlock::getStatements() {
  return mStatements;
}

GlobalStatementList& ProgramBlock::getGlobalStatements() {
  return mGlobalStatementList;
}
