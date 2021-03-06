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

#include "IOwnerVariable.hpp"
#include "IReferenceVariable.hpp"
#include "IStatement.hpp"
#include "IVariable.hpp"
#include "TryCatchInfo.hpp"

namespace wisey {
  
  class Model;
  
  /**
   * Represents block scope inside wisey program.
   *
   * Each scope has local variables associated with it
   */
  class Scope {
    std::map<std::string, IVariable*> mVariables;
    std::list<IReferenceVariable*> mReferenceVariables;
    std::list<IOwnerVariable*> mOwnerVariables;
    llvm::BasicBlock* mBreakToBlock;
    llvm::BasicBlock* mContinueToBlock;
    TryCatchInfo* mTryCatchInfo;
    const IType* mReturnType;
    std::map<std::string, int> mExceptions;
    
  public:
    
    Scope();
    
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
    void freeOwnedMemory(IRGenerationContext& context, llvm::Value* exception, int line);
    
    /**
     * Add an exception type that maybe thrown in this scope
     */
    void addException(const Model* exception, int line);
    
    /**
     * Add several exception types that may thrown in this scope
     */
    void addExceptions(std::vector<const Model*> exceptions, int line);
    
    /**
     * Remove an exception type from the list of exceptions that maybe thrown
     */
    void removeException(const Model* exception);
    
    /**
     * Get exceptions that could be thrown in this scope
     */
    std::map<std::string, int> getExceptions();
    
    /**
     * Tells whether the scope has any owner variables
     */
    bool hasOwnerVariables() const;
    
  };
  
} // namespace wisey

#endif /* Scope_h */

