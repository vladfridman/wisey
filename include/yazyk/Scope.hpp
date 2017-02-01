//
//  Scope.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Scope_h
#define Scope_h

#include "yazyk/Variable.hpp"

namespace yazyk {

/**
 * Represents block scope inside yazyk program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, Variable*> mLocals;
  llvm::BasicBlock* mBreakToBlock;
  llvm::BasicBlock* mContinueToBlock;
  bool mHasOwnedMemoryBeenFreed;

public:

  Scope() : mBreakToBlock(NULL), mContinueToBlock(NULL), mHasOwnedMemoryBeenFreed(false) { }
  
  /**ß
   * Returns map of local variables associated with this program block
   */
  std::map<std::string, Variable*>& getLocals();
  
  /**
   * Set block to break to out of a loop or a switch statement
   */
  void setBreakToBlock(llvm::BasicBlock* block);
  
  /**
   * Get the block to break to out of a loop or a switch statement
   */
  llvm::BasicBlock* getBreakToBlock();

  /**
   * Set block to continue to block for a loop
   */
  void setContinueToBlock(llvm::BasicBlock* block);
  
  /**
   * Get block to continue to block for a loop
   */
  llvm::BasicBlock* getContinueToBlock();

  /**
   * Free memory owned by this scope
   */
  void maybeFreeOwnedMemory(llvm::BasicBlock* block);
};
  
} // namespace yazyk

#endif /* Scope_h */
