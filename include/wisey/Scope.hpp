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
  
/**
 * Represents block scope inside wisey program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, IVariable*> mLocals;
  llvm::BasicBlock* mBreakToBlock;
  llvm::BasicBlock* mContinueToBlock;
  llvm::BasicBlock* mLandingPadBlock;
  llvm::BasicBlock* mExceptionContinueBlock;
  const IStatement* mExceptionFinally;
  bool mHasOwnedMemoryBeenFreed;
  IType* mReturnType;
  std::map<std::string, IType*> mExceptions;

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
  void setReturnType(IType* type);

  /**
   * Get current method's return type
   */
  IType* getReturnType();

  /**
   * Free memory owned by this scope
   */
  void freeOwnedMemory(IRGenerationContext& context);

  /**
   * Add an exception type that maybe thrown in this scope
   */
  void addException(IType* exception);

  /**
   * Add several exception types that may thrown in this scope
   */
  void addExceptions(std::vector<IType*> exceptions);

  /**
   * Remove an exception type from the list of exceptions that maybe thrown
   */
  void removeException(IType* exception);

  /**
   * Get exceptions that could be thrown in this scope
   */
  std::map<std::string, IType*> getExceptions();

};

} // namespace wisey

#endif /* Scope_h */
