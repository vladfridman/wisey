//
//  ContinueStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/ContinueStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace yazyk;

Value* ContinueStatement::generateIR(IRGenerationContext& context) const {
  BasicBlock* continueToBlock = context.getScopes().getContinueToBlock();
  
  if (continueToBlock == NULL) {
    Log::e("continue statement not inside a loop");
    exit(1);
  }
  return IRWriter::createBranch(context, continueToBlock);
}
