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

#include "IStatement.hpp"

namespace wisey {
  
  class Catch;
  
  typedef std::function<bool(IRGenerationContext&, llvm::BasicBlock*, std::vector<Catch*>,
  llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::LandingPadInst*, llvm::Value*,
  llvm::Value*)>
  LandingPadComposingFunction;
  
  /**
   * Information needed for IR generation of a try catch sequence
   */
  class TryCatchInfo {
    std::vector<Catch*> mCatchList;
    llvm::BasicBlock* mContinueBlock;
    std::vector<std::tuple<LandingPadComposingFunction, llvm::BasicBlock*, llvm::BasicBlock*,
    llvm::BasicBlock*, llvm::LandingPadInst*, llvm::Value*, llvm::Value*>> mComposingCallbacks;
    
  public:
    
    TryCatchInfo(std::vector<Catch*> catchList, llvm::BasicBlock* continueBlock);
    
    ~TryCatchInfo();
    
    /**
     * Returns the list of catch statements for this try/catch block
     */
    std::vector<Catch*> getCatchList();
    
    /**
     * Defines landing pad based block on try/catch statement that created this TryCatchInfo
     */
    std::tuple<llvm::BasicBlock*, llvm::Value*>
    defineLandingPadBlock(IRGenerationContext& context,
                          llvm::BasicBlock* freeMemoryBlock,
                          llvm::BasicBlock* freeMemoryEndBlock);
    
    /**
     * Run callbacks composing landing pads
     */
    bool runComposingCallbacks(IRGenerationContext& context);
    
  private:
    
    static bool composeLandingPadBlock(IRGenerationContext& context,
                                       llvm::BasicBlock* landingPadBlock,
                                       std::vector<Catch*> catchList,
                                       llvm::BasicBlock* continueBlock,
                                       llvm::BasicBlock* freeMemoryBlock,
                                       llvm::BasicBlock* freeMemoryEndBlock,
                                       llvm::LandingPadInst* landingPadInst,
                                       llvm::Value* wrappedException,
                                       llvm::Value* exceptionTypeId);
    
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

