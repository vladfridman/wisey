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

ProgramBlock::~ProgramBlock() {
  for (IGlobalStatement* statement : mGlobalStatements) {
    delete statement;
  }
  mGlobalStatements.clear();
}

void ProgramBlock::prototypeObjects(IRGenerationContext& context) const {
  for (IGlobalStatement* statement : mGlobalStatements) {
    statement->prototypeObjects(context);
  }
}

void ProgramBlock::prototypeMethods(IRGenerationContext& context) const {
  for (IGlobalStatement* statement : mGlobalStatements) {
    statement->prototypeMethods(context);
  }
}

Value* ProgramBlock::generateIR(IRGenerationContext& context) const {
  for (IGlobalStatement* statement : mGlobalStatements) {
    statement->generateIR(context);
  }
  return NULL;
}

GlobalStatementList& ProgramBlock::getGlobalStatements() {
  return mGlobalStatements;
}
