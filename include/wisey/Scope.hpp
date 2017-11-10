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

#include "wisey/IOwnerVariable.hpp"
#include "wisey/IReferenceVariable.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/TryCatchInfo.hpp"

namespace wisey {

class Model;
  
/**
 * Represents block scope inside wisey program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, IVariable*> mVariables;
  std::vector<IReferenceVariable*> mReferenceVariables;
  std::vector<IOwnerVariable*> mOwnerVariables;
  llvm::BasicBlock* mBreakToBlock;
  llvm::BasicBlock* mContinueToBlock;
  TryCatchInfo* mTryCatchInfo;
  bool mHasOwnedMemoryBeenFreed;
  const IType* mReturnType;
  const IObjectType* mObjectType;
  std::map<std::string, const Model*> mExceptions;

public:

  Scope() :
  mBreakToBlock(NULL),
  mContinueToBlock(NULL),
  mTryCatchInfo(NULL),
  mHasOwnedMemoryBeenFreed(false),
  mReturnType(NULL),
  mObjectType(NULL) { }
  
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
   * Get list of scope's cleared variables given the map of all cleared variables
   */
  std::vector<std::string> getClearedVariables(std::map<std::string, IVariable*>
                                               allClearedVariables);

  /**
   * Returns a list of reference variables from this scope
   */
  std::vector<IReferenceVariable*> getReferenceVariables();
  
  /**
   * Returns a list of owner variables from this scope
   */
  std::vector<IOwnerVariable*> getOwnerVariables();
  
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
   * Sets information about exception handling
   */
  void setTryCatchInfo(TryCatchInfo* tryCatchInfo);

  /**
   * Returns information about exception handling
   */
  TryCatchInfo* getTryCatchInfo();

  /**
   * Clear information about exception handling
   */
  void clearTryCatchInfo();
  
  /**
   * When processing object definition this keeps track which object type is being processed
   */
  void setObjectType(const IObjectType* objectType);
  
  /**
   * Gets type of the object definition that is being processed
   */
  const IObjectType* getObjectType() const;
  
  /**
   * Set current method's return type
   */
  void setReturnType(const IType* type);

  /**
   * Get current method's return type
   */
  const IType* getReturnType();

  /**
   * Free memory owner variables and decrement reference count for reference variables
   */
  void freeOwnedMemory(IRGenerationContext& context,
                       std::map<std::string, IVariable*>& clearedVariables);

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
