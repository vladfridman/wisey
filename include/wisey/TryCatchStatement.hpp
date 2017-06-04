//
//  TryCatchStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TryCatchStatement_h
#define TryCatchStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/Catch.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a try/catch statement
 */
class TryCatchStatement : public IStatement {
  IStatement* mTryBlock;
  std::vector<Catch*> mCatchList;
  const IStatement* mFinally;
  
public:
  
  TryCatchStatement(IStatement* tryBlock, std::vector<Catch*> catchList, const IStatement* finallyClause)
  : mTryBlock(tryBlock), mCatchList(catchList), mFinally(finallyClause) { }
  
  ~TryCatchStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::vector<std::tuple<Catch*, llvm::BasicBlock*>>
  generateSelectCatchByExceptionType(IRGenerationContext& context,
                                     llvm::BasicBlock* landingPadBlock,
                                     llvm::Value* exceptionTypeId) const;

  std::tuple<llvm::LandingPadInst*, llvm::Value*, llvm::Value*>
  generateLandingPad(IRGenerationContext& context, llvm::BasicBlock* landingPadBlock) const;
  
  void generateResumeAndFail(IRGenerationContext& context,
                             llvm::LandingPadInst* landingPadInst,
                             llvm::Value* exceptionTypeId,
                             llvm::Value* wrappedException) const;
  
  void generateCatches(IRGenerationContext& context,
                       llvm::Value* wrappedException,
                       std::vector<std::tuple<Catch*, llvm::BasicBlock*>> catchesAndBlocks) const;

};
  
} /* namespace wisey */

#endif /* TryCatchStatement_h */
