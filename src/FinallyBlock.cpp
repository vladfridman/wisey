//
//  FinallyBlock.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FinallyBlock.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* FinallyBlock::generateIR(IRGenerationContext& context) const {
  for (IStatement* statement : mStatements) {
    statement->generateIR(context);
  }

  return NULL;
}

StatementList& FinallyBlock::getStatements() {
  return mStatements;
}

