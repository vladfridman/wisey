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
  FinallyBlock* mFinallyBlock;
  std::vector<Catch*> mCatchList;
  llvm::BasicBlock* mContinueBlock;
  bool mDoAllCatchesTerminate;
  
public:
  
  TryCatchInfo(FinallyBlock* finallyBlock,
               std::vector<Catch*> catchList,
               llvm::BasicBlock* continueBlock) :
  mFinallyBlock(finallyBlock),
  mCatchList(catchList),
  mContinueBlock(continueBlock),
  mDoAllCatchesTerminate(false) { }
  
  ~TryCatchInfo();
  
  /**
   * Returns the list of statement that should always by executed for a try/catch block
   */
  FinallyBlock* getFinallyBlock();
  
  /**
   * Returns the list of catch statements for this try/catch block
   */
  std::vector<Catch*> getCatchList();
  
  /**
   * Generates landing pad based on try/catch statement that created this TryCatchInfo
   */
  llvm::BasicBlock* generateLandingPad(IRGenerationContext& context);
  
  /**
   * Tells whether all catches end with a terminator
   */
  bool doAllCatchesTerminate();

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

#endif /* TryCatchInfo_h */
