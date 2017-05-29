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
  for (IObjectDefinitionStatement* statement : mObjectDefinitions) {
    statement->prototypeObjects(context);
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

ObjectDefinitionStatementList& ProgramBlock::getObjectDefinitions() {
  return mObjectDefinitions;
}
