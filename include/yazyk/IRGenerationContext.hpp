//
//  IRGenerationContext.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IRGenerationContext_h
#define IRGenerationContext_h

#include <stack>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "yazyk/Scope.hpp"

namespace yazyk {
  
/**
 * Represents context for Intermidate Representation code generation
 */
class IRGenerationContext {
  llvm::LLVMContext mLLVMContext;
  std::vector<Scope *> mScopes;
  llvm::Function* mMainFunction;
  llvm::Module* mModule;
  std::unique_ptr<llvm::Module> mOwner;
  llvm::BasicBlock* mBasicBlock;
  
public:
  
  IRGenerationContext() {
    mOwner = llvm::make_unique<llvm::Module>("yazyk", mLLVMContext);
    mModule = mOwner.get();
    mMainFunction = NULL;
  }
  
  /**
   * Run compiled IR code and return the result
   */
  llvm::GenericValue runCode();
  
  /**
   * Return the main module
   */
  llvm::Module* getModule();

  /**
   * Returns scoped variable which could be defined either in the current scope or one of 
   * the parent scopes.
   */
  llvm::Value* getVariable(std::string name);
  
  /**
   * Set the local variable to a given value
   */
  void setVariable(std::string name, llvm::Value* value);
  
  /**
   * Sets the main function for the program
   */
  void setMainFunction(llvm::Function* function);
  
  /**
   * Returns the main function of the program
   */
  llvm::Function* getMainFunction();
    
  /**
   * Pushes a new program scope on the stack of program scopes
   */
  void pushScope();
  
  /**
   * Pops a program scope out of the stack
   */
  void popScope();
  
  /**
   * Returns current program scope
   */
  Scope* getCurrentScope() {
    return mScopes.back();
  }
  
  /**
   * Return current LLVM basic block
   */
  llvm::BasicBlock* getBasicBlock();
  
  /**
   * Set current LLVM basic block
   */
  void setBasicBlock(llvm::BasicBlock* block);

  /**
   * Set block to break to out of a loop or a switch statement
   */
  void setBreakToBlock(llvm::BasicBlock* block);
  
  /**
   * Get the block to break to out of a loop or a switch statement
   */
  llvm::BasicBlock* getBreakToBlock();
  
  /**
   * Returns the LLVMContext
   */
  llvm::LLVMContext& getLLVMContext();
  
  /**
   * Print LLVM assembly language of the IR program
   */
  void printAssembly(llvm::raw_ostream &outputStream);
  
  /**
   * Oprimize IR code
   * TODO: implement and add a unit test for it
   */
  void optimizeIR();
};

} /* namespace yazyk */

#endif /* IRGenerationContext_h */
