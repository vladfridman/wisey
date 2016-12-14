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
  
class Block;

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
    mOwner = llvm::make_unique<llvm::Module>("test", mLLVMContext);
    mModule = mOwner.get();
    mMainFunction = NULL;
  }
  
  llvm::Module * getModule() {
    return mModule;
  }
  
  void generateIR(Block& root);
  
  llvm::GenericValue runCode();
  
  std::map<std::string, llvm::Value*>& locals() {
    return mBlocks.top()->getLocals();
  }
  
  void setMainFunction(llvm::Function* function) {
    mMainFunction = function;
  }
  
  llvm::Function* getMainFunction() {
    return mMainFunction;
  }
  
  llvm::BasicBlock *currentBlock() {
    return mBlocks.top()->getBlock();
  }
  
  void replaceBlock(llvm::BasicBlock *block) {
    mBlocks.top()->setBlock(block);
  }
  
  void pushBlock(llvm::BasicBlock *block) {
    mBlocks.push(new IRGenerationBlock());
    mBlocks.top()->setBlock(block);
  }
  
  void popBlock() {
    IRGenerationBlock *top = mBlocks.top();
    mBlocks.pop();
    delete top;
  }
  
  llvm::LLVMContext & getLLVMContext() {
    return mLLVMContext;
  }
};

} /* namespace yazyk */

#endif /* IRGenerationContext_h */
