//
//  BreakStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/25/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/BreakStatement.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace yazyk;

Value* BreakStatement::generateIR(IRGenerationContext& context) const {
  BasicBlock* breackToBlock = context.getBreakToBlock();
  
  if (breackToBlock == NULL) {
    Log::e("break statement not inside a loop or a switch");
    exit(1);
  }
  return SafeBranch::newBranch(context.getBreakToBlock(), context);
}