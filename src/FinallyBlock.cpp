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
  Scope* scopeWithTryCatchInfo = NULL;
  TryCatchInfo* tryCatchInfo = NULL;
  for (Scope* scope : context.getScopes().getScopesList()) {
    TryCatchInfo* info = scope->getTryCatchInfo();
    if (info != NULL) {
      scopeWithTryCatchInfo = scope;
      tryCatchInfo = info;
      break;
    }
  }

  assert(tryCatchInfo);
  
  scopeWithTryCatchInfo->clearTryCatchInfo();
  for (IStatement* statement : mStatements) {
    statement->generateIR(context);
  }
  scopeWithTryCatchInfo->setTryCatchInfo(tryCatchInfo);

  return NULL;
}

StatementList& FinallyBlock::getStatements() {
  return mStatements;
}

