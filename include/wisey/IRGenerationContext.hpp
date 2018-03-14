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

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayExactType.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ImportProfile.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/Model.hpp"
#include "wisey/Node.hpp"
#include "wisey/PackageType.hpp"
#include "wisey/Scopes.hpp"
#include "wisey/Thread.hpp"

namespace wisey {
  
  typedef std::function<void(IRGenerationContext&, llvm::Function*)> ComposingFunction0Objects;
  typedef std::function<void(IRGenerationContext&, llvm::Function*, const IObjectType*)>
  ComposingFunction1Objects;
  typedef std::function<void(IRGenerationContext&, llvm::Function*, const IObjectType*,
  const IObjectType*)> ComposingFunction2Objects;
  
  /**
   * Represents context for Intermidate Representation code generation
   */
  class IRGenerationContext : public IPrintable {
    llvm::LLVMContext mLLVMContext;
    llvm::Function* mMainFunction;
    llvm::Module* mModule;
    std::unique_ptr<llvm::Module> mModuleOwner;
    llvm::BasicBlock* mBasicBlock;
    std::map<std::string, ArrayType*> mArrayTypes;
    std::map<std::string, ArrayExactType*> mArrayExactTypes;
    std::map<std::string, Model*> mModels;
    std::map<std::string, Controller*> mControllers;
    std::map<std::string, Node*> mNodes;
    std::map<std::string, Thread*> mThreads;
    std::map<std::string, Interface*> mInterfaces;
    std::map<std::string, PackageType*> mPackageTypes;
    std::map<Interface*, Controller*> mBindings;
    ImportProfile* mImportProfile;
    std::string mPackage;
    Scopes mScopes;
    bool mIsDestructorDebugOn;
    std::list<std::tuple<ComposingFunction0Objects, llvm::Function*>> mComposingCallbacks0Objects;
    std::list<std::tuple<ComposingFunction1Objects, llvm::Function*, const IObjectType*>>
    mComposingCallbacks1Objects;
    std::list<std::tuple<ComposingFunction2Objects, llvm::Function*, const IObjectType*,
    const IObjectType*>> mComposingCallbacks2Objects;
    const IObjectType* mObjectType;
    
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
     * Get an array type
     */
    wisey::ArrayType* getArrayType(const IType* elementType, unsigned long numberOfDimensions);
    
    /**
     * Get an array exact type for static array allocation such as { 1, 2, 3, 4 }
     */
    ArrayExactType* getArrayExactType(const IType* elementType,
                                      std::list<unsigned long> dimensions);
    
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
     * Add a thread type
     */
    void addThread(Thread* thread);
    
    /**
     * Look up a thread type
     */
    Thread* getThread(std::string name);
    
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
     * Returns a PackageType or creates one if one does not exist yet
     */
    PackageType* getPackageType(std::string packageName);
    
    /**
     * Sets current import profile that keeps track of all imports
     */
    void setImportProfile(ImportProfile* importProfile);
    
    /**
     * Returns current import profile
     */
    ImportProfile* getImportProfile() const;
    
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
    void addComposingCallback0Objects(ComposingFunction0Objects callback,
                                      llvm::Function* function);
    
    /**
     * Add a IR composition function callback that will be called after code generation is finished
     */
    void addComposingCallback1Objects(ComposingFunction1Objects callback,
                                      llvm::Function* function,
                                      const IObjectType* objectType);
    
    /**
     * Add a IR composition function callback that will be called after code generation is finished
     */
    void addComposingCallback2Objects(ComposingFunction2Objects callback,
                                      llvm::Function* function,
                                      const IObjectType* objectType1,
                                      const IObjectType* objectType2);
    
    /**
     * Run composition callbacks that compose auxilary function bodies
     */
    void runComposingCallbacks();
    
    /**
     * When processing object definition this keeps track which object type is being processed
     */
    void setObjectType(const IObjectType* objectType);
    
    /**
     * Gets type of the current object definition that is being processed
     */
    const IObjectType* getObjectType() const;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* IRGenerationContext_h */

