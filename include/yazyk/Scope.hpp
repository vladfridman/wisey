//
//  Scope.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Scope_h
#define Scope_h

#include <map>

#include "yazyk/IVariable.hpp"

namespace yazyk {

/**
 * Represents block scope inside yazyk program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, IVariable*> mLocals;
  llvm::BasicBlock* mBreakToBlock;
  llvm::BasicBlock* mContinueToBlock;
  bool mHasOwnedMemoryBeenFreed;

public:

  Scope() : mBreakToBlock(NULL), mContinueToBlock(NULL), mHasOwnedMemoryBeenFreed(false) { }
  
  /**
   * Finds a variable and returns it. Returns NULL if the variable is not found;
   */
  IVariable* findVariable(std::string name);
  
  /**
   * Sets a given variable
   */
  void setVariable(std::string name, IVariable* variable);
  
  /**
   * Clears a variable
   */
  void clearVariable(std::string name);
  
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
