//
//  Scope.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Scope_h
#define Scope_h

#include <map>

#include "wisey/IStatement.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

class Model;
  
/**
 * Represents block scope inside wisey program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, IVariable*> mVariables;
  llvm::BasicBlock* mBreakToBlock;
  llvm::BasicBlock* mContinueToBlock;
  llvm::BasicBlock* mLandingPadBlock;
  llvm::BasicBlock* mExceptionContinueBlock;
  const IStatement* mExceptionFinally;
  bool mHasOwnedMemoryBeenFreed;
  const IType* mReturnType;
  std::map<std::string, const Model*> mExceptions;

public:

  Scope() :
  mBreakToBlock(NULL),
  mContinueToBlock(NULL),
  mLandingPadBlock(NULL),
  mExceptionContinueBlock(NULL),
  mExceptionFinally(NULL),
  mHasOwnedMemoryBeenFreed(false),
  mReturnType(NULL) { }
  
  ~Scope();

  /**
   * Finds a variable and returns it. Returns NULL if the variable is not found;
   */
  IVariable* findVariable(std::string name);

  /**
   * Sets a given variable
   */
  void setVariable(std::string name, IVariable* variable);

  /**
   * Erases variables that were cleared from list of variables and from clearedVariables list
   */
  void eraseClearedVariables(std::map<std::string, IVariable*>& clearedVariables);

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
   * Sets the landing pad basic block
   */
  void setLandingPadBlock(llvm::BasicBlock* landingPadBlock);

  /**
   * Returns the landing pad basic block if there is one set
   */
  llvm::BasicBlock* getLandingPadBlock();

  /**
   * Sets the exception continue block that is the block after the last catch
   */
  void setExceptionContinueBlock(llvm::BasicBlock* basicBlock);

  /**
   * Returns the exception continue block that is the block after the last catch
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

  /**
   * Set current method's return type
   */
  void setReturnType(const IType* type);

  /**
   * Get current method's return type
   */
  const IType* getReturnType();

  /**
   * Free memory owned by this scope
   */
  void freeOwnedMemory(IRGenerationContext& context);

  /**
   * Add an exception type that maybe thrown in this scope
   */
  void addException(const Model* exception);

  /**
   * Add several exception types that may thrown in this scope
   */
  void addExceptions(std::vector<const Model*> exceptions);

  /**
   * Remove an exception type from the list of exceptions that maybe thrown
   */
  void removeException(const Model* exception);

  /**
   * Get exceptions that could be thrown in this scope
   */
  std::map<std::string, const Model*> getExceptions();

};

} // namespace wisey

#endif /* Scope_h */
