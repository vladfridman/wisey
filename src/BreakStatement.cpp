//
//  BreakStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/25/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/BreakStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

BreakStatement::BreakStatement(int line) : mLine(line) { }

BreakStatement::~BreakStatement() { }

void BreakStatement::generateIR(IRGenerationContext& context) const {
  BasicBlock* breackToBlock = context.getScopes().getBreakToBlock();
  
  if (breackToBlock == NULL) {
    context.reportError(mLine, "break statement not inside a loop or a switch");
    exit(1);
  }
  IRWriter::createBranch(context, breackToBlock, mLine);
}

int BreakStatement::getLine() const {
  return mLine;
}
