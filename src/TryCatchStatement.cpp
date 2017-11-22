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
#include "wisey/TryCatchInfo.hpp"
#include "wisey/TryCatchStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TryCatchStatement::~TryCatchStatement() {
  delete mTryBlock;
  for (Catch* catchClause : mCatchList) {
    delete catchClause;
  }
  mCatchList.clear();
  delete mFinallyStatement;
}

Value* TryCatchStatement::generateIR(IRGenerationContext& context) const {
  context.getScopes().pushScope();
  
  FinallyBlock finallyBlock;
  finallyBlock.getStatements().push_back(mFinallyStatement);
  if (context.getScopes().getTryCatchInfo()) {
    FinallyBlock* outerFinallyBlock = context.getScopes().getTryCatchInfo()->getFinallyBlock();
    finallyBlock.getStatements().insert(finallyBlock.getStatements().end(),
                                        outerFinallyBlock->getStatements().begin(),
                                        outerFinallyBlock->getStatements().end());
  }
  Function* function = context.getBasicBlock()->getParent();
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* continueBlock = BasicBlock::Create(llvmContext, "eh.continue", function);

  TryCatchInfo tryCatchInfo(&finallyBlock, mCatchList, continueBlock);
  context.getScopes().beginTryCatch(&tryCatchInfo);
  mTryBlock->generateIR(context);
  bool doesTryBlockTerminate = context.getBasicBlock()->getTerminator() != NULL;
  context.getScopes().endTryCatch();
  finallyBlock.generateIR(context);

  IRWriter::createBranch(context, continueBlock);

  context.getScopes().popScope(context);
  
  context.setBasicBlock(continueBlock);
  if (doesTryBlockTerminate && tryCatchInfo.doAllCatchesTerminate()) {
    IRWriter::newUnreachableInst(context);
  }
  
  return NULL;
}
