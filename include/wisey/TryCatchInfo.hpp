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
  
typedef std::function<bool(IRGenerationContext&, llvm::BasicBlock*, std::vector<Catch*>,
  llvm::BasicBlock*, llvm::BasicBlock*)> LandingPadComposingFunction;

/**
 * Information needed for IR generation of a try catch sequence
 */
class TryCatchInfo {
  std::vector<Catch*> mCatchList;
  llvm::BasicBlock* mContinueBlock;
  std::vector<std::tuple<LandingPadComposingFunction, llvm::BasicBlock*, llvm::BasicBlock*>>
    mComposingCallbacks;

public:
  
  TryCatchInfo(std::vector<Catch*> catchList, llvm::BasicBlock* continueBlock) :
  mCatchList(catchList),
  mContinueBlock(continueBlock) { }
  
  ~TryCatchInfo();
  
  /**
   * Returns the list of catch statements for this try/catch block
   */
  std::vector<Catch*> getCatchList();
  
  /**
   * Defines landing pad based block on try/catch statement that created this TryCatchInfo
   */
  llvm::BasicBlock* defineLandingPadBlock(IRGenerationContext& context,
                                          llvm::BasicBlock* freeMemoryBlock);
  
  /**
   * Run callbacks composing landing pads
   */
  bool runComposingCallbacks(IRGenerationContext& context);
  
private:
  
  static bool composeLandingPadBlock(IRGenerationContext& context,
                                     llvm::BasicBlock* landingPadBlock,
                                     std::vector<Catch*> catchList,
                                     llvm::BasicBlock* continueBlock,
                                     llvm::BasicBlock* freeTryMemoryBlock);
  
  static std::vector<std::tuple<Catch*, llvm::BasicBlock*>>
  generateSelectCatchByExceptionType(IRGenerationContext& context,
                                     llvm::Value* exceptionTypeId,
                                     std::vector<Catch*> catchList,
                                     llvm::BasicBlock* landingPadBlock);
  
  static std::tuple<llvm::LandingPadInst*, llvm::Value*, llvm::Value*>
  generateLandingPad(IRGenerationContext& context,
                     llvm::BasicBlock* landingPadBlock,
                     std::vector<Catch*> catchList);
  
  static void generateResumeAndFail(IRGenerationContext& context,
                                    llvm::LandingPadInst* landingPadInst,
                                    llvm::Value* exceptionTypeId,
                                    llvm::Value* wrappedException);
  
  static bool generateCatches(IRGenerationContext& context,
                              llvm::Value* wrappedException,
                              std::vector<std::tuple<Catch*, llvm::BasicBlock*>> catchesAndBlocks,
                              llvm::BasicBlock* exceptionContinueBlock);

};

} /* namespace wisey */

#endif /* TryCatchInfo_h */
