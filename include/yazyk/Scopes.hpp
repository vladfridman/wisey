//
//  Scopes.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Scopes_h
#define Scopes_h

#include "yazyk/Scope.hpp"

namespace yazyk {

/**
 * Methods for operating scopes and everything that relates to scopes
 */
class Scopes {
  std::vector<Scope *> mScopes;

public:
  Scopes() {}
  
  ~Scopes() {}
  
  /**
   * Returns scoped variable which could be defined either in the current scope or one of
   * the parent scopes.
   */
  llvm::Value* getVariable(std::string name);
  
  /**
   * Set the local stack variable to a given value
   */
  void setStackVariable(std::string name, llvm::Value* value);
  
  /**
   * Set the local stack variable to a given value
   */
  void setHeapVariable(std::string name, llvm::Value* value);
  
  /**
   * Pushes a new program scope on the stack of program scopes
   */
  void pushScope();
  
  /**
   * Pops a program scope out of the stack
   */
  void popScope(llvm::BasicBlock* basicBlock);
  
  /**
   * Returns current Scope
   */
  Scope* getScope();
  
  /**
   * Set block to break to out of a loop or a switch statement
   */
  void setBreakToBlock(llvm::BasicBlock* block);
  
  /**
   * Get the block to break to out of a loop or a switch statement
   */
  llvm::BasicBlock* getBreakToBlock();
  
  /**
   * Set continue to block for a loop
   */
  void setContinueToBlock(llvm::BasicBlock* block);
  
  /**
   * Get continue to block for a loop
   */
  llvm::BasicBlock* getContinueToBlock();

};
  
} /* namespace yazyk */

#endif /* Scopes_h */
