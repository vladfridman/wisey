//
//  TryCatchStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "wisey/Cleanup.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/TryCatchInfo.hpp"
#include "wisey/TryCatchStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TryCatchStatement::TryCatchStatement(IStatement* tryBlock, vector<Catch*> catchList) :
mTryBlock(tryBlock),
mCatchList(catchList) { }

TryCatchStatement::~TryCatchStatement() {
  delete mTryBlock;
  for (Catch* catchClause : mCatchList) {
    delete catchClause;
  }
  mCatchList.clear();
}

Value* TryCatchStatement::generateIR(IRGenerationContext& context) const {
  context.getScopes().pushScope();
  
  if (context.getScopes().getTryCatchInfo()) {
    Log::e("Nested try blocks are not allowed. Extract inner try/catch into a method.");
    exit(1);
  }
  
  Function* function = context.getBasicBlock()->getParent();
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* continueBlock = BasicBlock::Create(llvmContext, "eh.continue", function);

  TryCatchInfo tryCatchInfo(mCatchList, continueBlock);
  context.getScopes().beginTryCatch(&tryCatchInfo);
  mTryBlock->generateIR(context);
  bool doesTryBlockTerminate = context.getBasicBlock()->getTerminator() != NULL;
  bool doAllCatchesTerminate = context.getScopes().endTryCatch(context);

  IRWriter::createBranch(context, continueBlock);

  context.getScopes().popScope(context);
  
  context.setBasicBlock(continueBlock);
  if (doesTryBlockTerminate && doAllCatchesTerminate) {
    IRWriter::newUnreachableInst(context);
  }
  
  return NULL;
}
