//
//  IRGenerationContext.hpp
//  Wisey
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

#include "wisey/Controller.hpp"
#include "wisey/Interface.hpp"
#include "wisey/Model.hpp"
#include "wisey/Scopes.hpp"

namespace wisey {
  
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
  std::map<Interface*, Controller*> mBindings;
  std::map<std::string, IObjectType*> mImports;
  std::string mPackage;
  Scopes mScopes;

public:
  
  IRGenerationContext() : mMainFunction(NULL), mBasicBlock(NULL) {
    mOwner = llvm::make_unique<llvm::Module>("wisey", mLLVMContext);
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
  void addController(Controller* controller);

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
   * Bind an interface to a controller for injection
   */
  void bindInterfaceToController(Interface* interface, Controller* controller);
  
  /**
   * Returns controller bound to the given interface
   */
  Controller* getBoundController(Interface* interface);
  
  /**
   * Set the package name
   */
  void setPackage(std::string package);
  
  /**
   * Get the current package name
   */
  std::string getPackage();
  
  /**
   * Add an import that adds an alias from object's short name to the object
   */
  void addImport(IObjectType* object);
  
  /**
   * Returns an imported object given its short name
   */
  IObjectType* getImport(std::string objectName);
  
  /**
   * Clears the imports map and adds default imports
   */
  void clearAndAddDefaultImports();
  
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

} /* namespace wisey */

#endif /* IRGenerationContext_h */
