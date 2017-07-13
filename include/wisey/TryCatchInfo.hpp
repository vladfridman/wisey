//
//  TryCatchInfo.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TryCatchInfo_h
#define TryCatchInfo_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Information needed for IR generation of a try catch sequence
 */
class TryCatchInfo {
  llvm::BasicBlock* mLandingPadBlock;
  llvm::BasicBlock* mContinueBlock;
  const IStatement* mFinallyStatement;
  
public:
  
  TryCatchInfo(llvm::BasicBlock* landingPadBlock,
               llvm::BasicBlock* continueBlock,
               const IStatement* finallyStatement) :
  mLandingPadBlock(landingPadBlock),
  mContinueBlock(continueBlock),
  mFinallyStatement(finallyStatement) { }
  
  ~TryCatchInfo() {}
  
  /**
   * Returns the landing pad basic block for exception handling 
   */
  llvm::BasicBlock* getLandingPadBlock();
  
  /**
   * Returns the basic block for instructions after the try/catch
   */
  llvm::BasicBlock* getContinueBlock();
  
  /**
   * Returns the statement that should always by executed for a try/catch block
   */
  const IStatement* getFinallyStatement();
  
};

} /* namespace wisey */

#endif /* TryCatchInfo_h */
