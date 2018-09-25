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

#include "IType.hpp"
#include "IStatement.hpp"
#include "IVariable.hpp"
#include "Scope.hpp"
#include "TryCatchInfo.hpp"

namespace wisey {
  
  /**
   * Methods for operating scopes and everything that relates to scopes
   */
  class Scopes {
    std::list<Scope *> mScopes;
    llvm::BasicBlock* mCachedLandingPadBlock;
    
  public:
    
    Scopes();
    
    ~Scopes();
    
    /**
     * Returns scoped variable which could be defined either in the current scope or one of
     * the parent scopes.
     */
    IVariable* getVariable(std::string name);
    
    /**
     * Add a variable to the scope
     */
    void setVariable(IRGenerationContext& context, IVariable* variable);
    
    /**
     * Pushes a new program scope on the stack of program scopes
     */
    void pushScope();
    
    /**
     * Pops a program scope out of the stack
     */
    void popScope(IRGenerationContext& context, int line);
    
    /**
     * Returns current Scope
     */
    Scope* getScope();
    
    /**
     * Returns a reference to all scopes
     */
    std::list<Scope *> getScopesList();
    
    /**
     * Free memory owned by variables in all scopes in the scope stack.
     *
     * This is typically called before a return or a throw
     */
    void freeOwnedMemory(IRGenerationContext& context, llvm::Value* exception, int line);
    
    /**
     * Tells whether there are any owner variables in any of the scopes
     */
    bool hasOwnerVariables() const;

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
    void beginTryCatch(TryCatchInfo* tryCatchInfo);
    
    /**
     * Returns the exception handling info
     */
    TryCatchInfo* getTryCatchInfo();
    
    /**
     * Finishes try/catch processing by genereating catches and finally
     */
    bool endTryCatch(IRGenerationContext& context);
    
    /**
     * Get the landing pad for invoke call. This may generate a new ladning pad or return a cached one
     */
    llvm::BasicBlock* getLandingPadBlock(IRGenerationContext& context, int line);
    
    /**
     * Clears the scopes and forgets all variables
     */
    void clear();
    
  private:
    
    void reportUnhandledExceptions(IRGenerationContext& context,
                                   std::map<std::string, int> exceptions) const;
    
    void clearCachedLandingPadBlock();
    
    void freeMemoryAllocatedInTry(IRGenerationContext& context,
                                  llvm::BasicBlock* freeMemoryBlock,
                                  llvm::BasicBlock* freeMemoryEndBlock,
                                  llvm::Value* wrappedException,
                                  int line);

  };
  
} /* namespace wisey */

#endif /* Scopes_h */

