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
#include "wisey/FinallyBlock.hpp"
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
  IStatement* mFinallyStatement;
  
public:
  
  TryCatchStatement(IStatement* tryBlock,
                    std::vector<Catch*> catchList,
                    IStatement* finallyStatement) :
  mTryBlock(tryBlock), 
  mCatchList(catchList), 
  mFinallyStatement(finallyStatement) { }
  
  ~TryCatchStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  std::vector<std::tuple<Catch*, llvm::BasicBlock*>>
  generateSelectCatchByExceptionType(IRGenerationContext& context,
                                     llvm::Value* exceptionTypeId,
                                     std::vector<Catch*> allCatches,
                                     llvm::BasicBlock* landingPadBlock) const;

  std::tuple<llvm::LandingPadInst*, llvm::Value*, llvm::Value*>
  generateLandingPad(IRGenerationContext& context,
                     llvm::BasicBlock* landingPadBlock,
                     std::vector<Catch*> allCatches) const;
  
  void generateResumeAndFail(IRGenerationContext& context,
                             llvm::LandingPadInst* landingPadInst,
                             llvm::Value* exceptionTypeId,
                             llvm::Value* wrappedException,
                             FinallyBlock* finallyBlock) const;
  
  bool generateCatches(IRGenerationContext& context,
                       llvm::Value* wrappedException,
                       std::vector<std::tuple<Catch*, llvm::BasicBlock*>> catchesAndBlocks,
                       llvm::BasicBlock* exceptionContinueBlock,
                       FinallyBlock* finallyBlock) const;

};
  
} /* namespace wisey */

#endif /* TryCatchStatement_h */
