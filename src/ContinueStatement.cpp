//
//  ContinueStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ContinueStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

ContinueStatement::ContinueStatement(int line) : mLine(line) { }

ContinueStatement::~ContinueStatement() { }

void ContinueStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  
  BasicBlock* continueToBlock = context.getScopes().getContinueToBlock();
  
  if (continueToBlock == NULL) {
    context.reportError(mLine, "continue statement not inside a loop");
    throw 1;
  }
  IRWriter::createBranch(context, continueToBlock);
}
