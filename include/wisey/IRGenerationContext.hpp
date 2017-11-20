//
//  IRGenerationContext.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IRGenerationContext_h
#define IRGenerationContext_h

#include <iostream>
#include <stack>

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "wisey/Controller.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/Model.hpp"
#include "wisey/Node.hpp"
#include "wisey/Scopes.hpp"

namespace wisey {
  
typedef std::function<void(IRGenerationContext&, llvm::Function*, const IObjectType*)>
  ComposingFunction;
  
/**
 * Represents context for Intermidate Representation code generation
 */
class IRGenerationContext : public IPrintable {
  llvm::LLVMContext mLLVMContext;
  llvm::Function* mMainFunction;
  llvm::Module* mModule;
  std::unique_ptr<llvm::Module> mModuleOwner;
  llvm::BasicBlock* mBasicBlock;
  std::map<std::string, Model*> mModels;
  std::map<std::string, Controller*> mControllers;
  std::map<std::string, Node*> mNodes;
  std::map<std::string, Interface*> mInterfaces;
  std::map<Interface*, Controller*> mBindings;
  std::map<std::string, IObjectType*> mImports;
  std::string mPackage;
  llvm::Value* mSourceFileConstantPointer;
  Scopes mScopes;
  bool mIsDestructorDebugOn;
  std::vector<std::tuple<ComposingFunction, llvm::Function*, const IObjectType*>>
    mComposingCallbacks;

public:
  
  IRGenerationContext();
  
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
   * Add a model type
   */
  void addModel(Model* model);
  
  /**
   * Look up a model type
   */
  Model* getModel(std::string name);

  /**
   * Add a controller type
   */
  void addController(Controller* controller);

  /**
   * Look up a controller type
   */
  Controller* getController(std::string name);
  
  /**
   * Add a node type
   */
  void addNode(Node* node);
  
  /**
   * Look up a node type
   */
  Node* getNode(std::string name);
  
  /**
   * Add an interface type
   */
  void addInterface(Interface* interface);

  /**
   * Look up an interface type
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
   * Tells whether there is a controller bound to the given interface
   */
  bool hasBoundController(Interface* interface);

  /**
   * Set the package name
   */
  void setPackage(std::string package);
  
  /**
   * Get the current package name
   */
  std::string getPackage() const;
  
  /**
   * Sets the current source file being processed
   */
  void setSourceFileNamePointer(llvm::Value* sourceFileConstantPointer);
  
  /**
   * Returns the current source file being processed
   */
  llvm::Value* getSourceFileNamePointer() const;

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
   * Returns 'this' variable
   */
  IVariable* getThis();
  
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
  
  /**
   * Turns on destrcutor debug mode on that prints out a message when object destructor is called
   */
  void turnDestructorDebugOn();
  
  /**
   * Tells whether the program is compiled with destructor debugging mode on
   */
  bool isDestructorDebugOn() const;
  
  /**
   * Add a IR composition function callback that will be called after code generation is finished
   */
  void addComposingCallback(ComposingFunction callback,
                            llvm::Function* function,
                            const IObjectType* objectType);
  
  /**
   * Run composition callbacks that compose auxilary function bodies
   */
  void runComposingCallbacks();
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:

  IObjectType* getImportWithFail(std::string objectName);

};

} /* namespace wisey */

#endif /* IRGenerationContext_h */
