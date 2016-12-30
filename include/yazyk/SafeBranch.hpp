//
//  SafeBranch.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/29/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef SafeBranch_h
#define SafeBranch_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {

/**
 * Methods for safely creating branches avoiding adding a branch after the basic block has been
 * terminated.
 */
class SafeBranch {

public:
  
  /**
   * Safely create a branch to the given block.
   * 
   * If the current block's last isntruction is an instance of TerminatorInst then do nothing
   */
  static llvm::BranchInst* newBranch(llvm::BasicBlock* toBlock,
                                     yazyk::IRGenerationContext& context);

  /**
   * Safely create a conditional branch.
   *
   * If the current block's last instruction is an instance of TerminatorInst then do nothing
   */
  static llvm::BranchInst* newConditionalBranch(llvm::BasicBlock* ifTrueBlock,
                                                llvm::BasicBlock* ifFalseBlock,
                                                llvm::Value* condition,
                                                yazyk::IRGenerationContext& context);

  
};

} /* namespace yazyk */

#endif /* SafeBranch_h */
