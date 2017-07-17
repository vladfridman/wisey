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
#include "wisey/TryCatchInfo.hpp"

namespace wisey {
  
/**
 * Methods for operating scopes and everything that relates to scopes
 */
class Scopes {
  std::list<Scope *> mScopes;
  std::map<std::string, IVariable*> mClearedVariables;

public:
  Scopes() {}

  ~Scopes() {}

  /**
   * Returns scoped variable which could be defined either in the current scope or one of
   * the parent scopes.
   */
  IVariable* getVariable(std::string name);
  
  /**
   * Same as getVariable() but also returns cleared variables
   */
  IVariable* getVariableForAssignement(std::string name);

  /**
   * Remove reference to a variable
   */
  void clearVariable(IRGenerationContext& context, std::string name);

  /**
   * Add a variable to the scope
   */
  void setVariable(IVariable* variable);

  /**
   * Returns the map of cleared variables
   */
  std::map<std::string, IVariable*> getClearedVariables();
  
  /**
   * Sets the map of cleared variables to the given one
   */
  void setClearedVariables(std::map<std::string, IVariable*> clearedVariables);
  
  /**
   * Erase from cleared variables the given variable
   */
  void eraseFromClearedVariables(IVariable* variable);
  
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
   * Free memory owned by variables in all scopes in the scope stack.
   *
   * This is typically called before a return or a throw
   */
  void freeOwnedMemory(IRGenerationContext& context);

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
  void setReturnType(const IType* type);

  /**
   * Get current method's return type
   */
  const IType* getReturnType();

  /**
   * Sets information for exception handling
   */
  void setTryCatchInfo(TryCatchInfo* tryCatchInfo);

  /**
   * Returns the exception handling info
   */
  TryCatchInfo* getTryCatchInfo();
  
  /**
   * Clear information about exception handling
   */
  void clearTryCatchInfo();
  
  /**
   * Merge catches from scopes above with catches in the current scope
   */
  std::vector<Catch*> mergeNestedCatchLists(IRGenerationContext& context);

private:

  void reportUnhandledExceptions(std::map<std::string, const Model*> exceptions);
};
  
} /* namespace wisey */

#endif /* Scopes_h */
