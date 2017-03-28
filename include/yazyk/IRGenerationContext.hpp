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

#include "yazyk/Controller.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/Scopes.hpp"

namespace yazyk {
  
/**
 * Represents context for Intermidate Representation code generation
 */
class IRGenerationContext {
  llvm::LLVMContext mLLVMContext;
  llvm::Function* mMainFunction;
  llvm::Module* mModule;
  std::unique_ptr<llvm::Module> mOwner;
  llvm::BasicBlock* mBasicBlock;
  std::map<std::string, Model*> mModels;
  std::map<std::string, Controller*> mControllers;
  std::map<std::string, Interface*> mInterfaces;
  std::map<std::string, IType*> mGlobalFunctions;
  Scopes mScopes;

public:
  
  IRGenerationContext() : mMainFunction(NULL), mBasicBlock(NULL) {
    mOwner = llvm::make_unique<llvm::Module>("yazyk", mLLVMContext);
    mModule = mOwner.get();
  }
  
  ~IRGenerationContext();
  
  /**
   * Run compiled IR code and return the result
   */
  llvm::GenericValue runCode();
  
  /**
   * Return the main module
   */
  llvm::Module* getModule();
  
  /**
   * Sets the main function for the program
   */
  void setMainFunction(llvm::Function* function);
  
  /**
   * Returns the main function of the program
   */
  llvm::Function* getMainFunction();
  
  /**
   * Return current LLVM basic block
   */
  llvm::BasicBlock* getBasicBlock();
  
  /**
   * Set current LLVM basic block
   */
  void setBasicBlock(llvm::BasicBlock* block);

  /**
   * Add a MODEL type
   */
  void addModel(Model* model);
  
  /**
   * Look up a MODEL type
   */
  Model* getModel(std::string name);

  /**
   * Add a Controller type
   */
  void addController(Controller* model);
  
  /**
   * Look up a Controller type
   */
  Controller* getController(std::string name);
  
  /**
   * Add an Interface type
   */
  void addInterface(Interface* interface);
  
  /**
   * Look up an Interface type
   */
  Interface* getInterface(std::string name);
  
  /**
   * Add a global function
   */
  void addGlobalFunction(yazyk::IType* returnType, std::string functionName);
  
  /**
   * Get the return type of a global function
   */
  IType* getGlobalFunctionType(std::string functionName);
  
  /**
   * Return Scopes controller
   */
  Scopes& getScopes();
  
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
