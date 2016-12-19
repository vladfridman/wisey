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

#include "yazyk/IRGenerationBlock.hpp"

namespace yazyk {
  
/**
 * Represents context for Intermidate Representation code generation
 */
class IRGenerationContext {
  llvm::LLVMContext mLLVMContext;
  std::stack<IRGenerationBlock *> mBlocks;
  llvm::Function* mMainFunction;
  llvm::Module* mModule;
  std::unique_ptr<llvm::Module> mOwner;
  
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
   * Returns the map of local variables for the current program block
   */
  std::map<std::string, llvm::Value*>& locals();
  
  /**
   * Sets the main function for the program
   */
  void setMainFunction(llvm::Function* function);
  
  /**
   * Returns the main function of the program
   */
  llvm::Function* getMainFunction();
  
  /**
   * Returns the most recent program block
   */
  llvm::BasicBlock *currentBlock();
  
  /**
   * Replaces the current program block with the given one
   */
  void replaceBlock(llvm::BasicBlock *block);
  
  /**
   * Pushes a new program block on the stack of program blocks
   */
  void pushBlock(llvm::BasicBlock *block);
  
  /**
   * Pops a program block out of the stack
   */
  void popBlock();
  
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
