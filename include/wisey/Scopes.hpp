//
//  Scopes.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Scopes_h
#define Scopes_h

#include <list>

#include "wisey/IType.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/Scope.hpp"

namespace wisey {
  
/**
 * Methods for operating scopes and everything that relates to scopes
 */
class Scopes {
  std::list<Scope *> mScopes;

public:
  Scopes() {}

  ~Scopes() {}

  /**
   * Returns scoped variable which could be defined either in the current scope or one of
   * the parent scopes.
   */
  IVariable* getVariable(std::string name);

  /**
   * Remove reference to a variable
   */
  void clearVariable(std::string name);

  /**
   * Add a variable to the scope
   */
  void setVariable(IVariable* variable);

  /**
   * Pushes a new program scope on the stack of program scopes
   */
  void pushScope();

  /**
   * Pops a program scope out of the stack
   */
  void popScope(IRGenerationContext& context);

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

  /**
   * Set current method's return type
   */
  void setReturnType(IType* type);

  /**
   * Get current method's return type
   */
  IType* getReturnType();

  /**
   * Sets the landing pad basic block for exception handling
   */
  void setLandingPadBlock(llvm::BasicBlock* basicBlock);

  /**
   * Returns the landing pad basic block for exception handling if there is one set
   */
  llvm::BasicBlock* getLandingPadBlock();

  /**
   * Set the basic block for instructions after the try/catch
   */
  void setExceptionContinueBlock(llvm::BasicBlock* basicBlock);

  /**
   * Returns the basic block for instructions after the try/catch
   */
  llvm::BasicBlock* getExceptionContinueBlock();

  /**
   * Set the statement that should always by executed for a try/catch block
   */
  void setExceptionFinally(const IStatement* finallyStatement);
  
  /**
   * Returns the statement that should always by executed for a try/catch block
   */
  const IStatement* getExceptionFinally();

private:

  void reportUnhandledExceptions(std::map<std::string, IType*> exceptions);
};
  
} /* namespace wisey */

#endif /* Scopes_h */