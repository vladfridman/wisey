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

class Catch;
class FinallyBlock;
  
/**
 * Information needed for IR generation of a try catch sequence
 */
class TryCatchInfo {
  llvm::BasicBlock* mLandingPadBlock;
  FinallyBlock* mFinallyBlock;
  std::vector<Catch*> mCatchList;
  
public:
  
  TryCatchInfo(llvm::BasicBlock* landingPadBlock,
               FinallyBlock* finallyBlock,
               std::vector<Catch*> catchList) :
  mLandingPadBlock(landingPadBlock),
  mFinallyBlock(finallyBlock),
  mCatchList(catchList) { }
  
  ~TryCatchInfo();
  
  /**
   * Returns the landing pad basic block for exception handling 
   */
  llvm::BasicBlock* getLandingPadBlock();
  
  /**
   * Returns the list of statement that should always by executed for a try/catch block
   */
  FinallyBlock* getFinallyBlock();
  
  /**
   * Returns the list of catch statements for this try/catch block
   */
  std::vector<Catch*> getCatchList();
  
};

} /* namespace wisey */

#endif /* TryCatchInfo_h */
