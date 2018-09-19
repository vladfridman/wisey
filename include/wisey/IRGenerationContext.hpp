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
#include "wisey/LLVMArrayType.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/LLVMStructType.hpp"
#include "wisey/Model.hpp"
#include "wisey/Node.hpp"
#include "wisey/PackageType.hpp"
#include "wisey/Scopes.hpp"

namespace wisey {
  
  typedef std::function<void(IRGenerationContext&, llvm::Function*)> ComposingFunction0Objects;
  typedef std::function<void(IRGenerationContext&, llvm::Function*, const void*)>
  ComposingFunction1Objects;
  typedef std::function<void(IRGenerationContext&, llvm::Function*, const void*, const void*)>
  ComposingFunction2Objects;
  
  /**
   * Represents context for Intermidate Representation code generation
   */
  class IRGenerationContext : public IPrintable {
    llvm::LLVMContext mLLVMContext;
    llvm::Module* mModule;
    std::unique_ptr<llvm::Module> mModuleOwner;
    llvm::BasicBlock* mBasicBlock;
    llvm::BasicBlock* mDeclarationsBlock;
    std::map<std::string, ArrayType*> mArrayTypes;
    std::map<std::string, ArrayExactType*> mArrayExactTypes;
    std::map<std::string, LLVMArrayType*> mLLVMArrayTypes;
    std::map<std::string, Model*> mModels;
    std::map<std::string, Controller*> mControllers;
    std::map<std::string, Node*> mNodes;
    std::map<std::string, Interface*> mInterfaces;
    std::map<std::string, PackageType*> mPackageTypes;
    std::map<std::string, LLVMStructType*> mLLVMStructTypes;
    std::map<std::string, LLVMFunctionType*> mLLVMFunctionTypes;
    std::map<std::string, const LLVMFunctionType*> mLLVMInternalFunctionNamedTypes;
    std::map<std::string, const LLVMFunctionType*> mLLVMExternalFunctionNamedTypes;
    std::map<std::string, const IType*> mGlobalVariables;
    std::map<std::string, std::tuple<std::string, int>> mBindings;
    ImportProfile* mImportProfile;
    std::string mPackage;
    Scopes mScopes;
    bool mIsDestructorDebugOn;
    bool mShouldGenerateMain;
    std::list<std::tuple<ComposingFunction0Objects, llvm::Function*>> mComposingCallbacks0Objects;
    std::list<std::tuple<ComposingFunction1Objects, llvm::Function*, const void*>>
    mComposingCallbacks1Objects;
    std::list<std::tuple<ComposingFunction2Objects, llvm::Function*, const void*, const void*>>
    mComposingCallbacks2Objects;
    const IObjectType* mObjectType;
    const IMethod* mCurrentMethod;
    bool mIsRunningComposingCallbacks;
    
  public:
    
    IRGenerationContext();
    
    ~IRGenerationContext();
    
    /**
     * Run compiled IR code and return the result
     */
    int runCode(int argc, char** argv);
    
    /**
     * Return the main module
     */
    llvm::Module* getModule();
    
    /**
     * Return current LLVM basic block
     */
    llvm::BasicBlock* getBasicBlock();
    
    /**
     * Set current LLVM basic block
     */
    void setBasicBlock(llvm::BasicBlock* block);
    
    /**
     * Returns LLVM basic block where all variable declarations are placed
     */
    llvm::BasicBlock* getDeclarationsBlock();
    
    /**
     * Set LLVM basic block where all variable declarations will be placed
     */
    void setDeclarationsBlock(llvm::BasicBlock* block);

    /**
     * Resets the context to clean state
     */
    void reset();
    
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
     * Get an llvm array type
     */
    LLVMArrayType* getLLVMArrayType(const IType* elementType,
                                    std::list<unsigned long> dimensions);

    /**
     * Add a model type
     */
    void addModel(Model* model, int line);
    
    /**
     * Look up a model type
     */
    Model* getModel(std::string name, int line) const;
    
    /**
     * Add a controller type
     */
    void addController(Controller* controller, int line);
    
    /**
     * Look up a controller type
     */
    Controller* getController(std::string name, int line) const;
    
    /**
     * Add a node type
     */
    void addNode(Node* node, int line);
    
    /**
     * Look up a node type
     */
    Node* getNode(std::string name, int line) const;
    
    /**
     * Add an interface type
     */
    void addInterface(Interface* interface, int line);
    
    /**
     * Look up an interface type
     */
    Interface* getInterface(std::string name, int line) const;
    
    /**
     * Add an llvm struct type
     */
    void addLLVMStructType(LLVMStructType* llvmStructType, int line);
    
    /**
     * Look up a llvm struct type
     */
    LLVMStructType* getLLVMStructType(std::string name, int line);
    
    /**
     * Look up a llvm function type
     */
    LLVMFunctionType* getLLVMFunctionType(const IType* returnType,
                                          std::vector<const IType*> argumentTypes);
    
    /**
     * Look up a llvm function with variable arguments type
     */
    LLVMFunctionType* getLLVMFunctionTypeWithVarArg(const IType* returnType,
                                                    std::vector<const IType*> argumentTypes);

    /**
     * Set global llvm variable
     */
    void setLLVMGlobalVariable(const IType* type, std::string name);
    
    /**
     * Returns type of the global llvm variable
     */
    const IType* getLLVMGlobalVariableType(std::string name) const;
    
    /**
     * Registers an internal llvm function with the given type and name
     */
    void registerLLVMInternalFunctionNamedType(std::string name,
                                               const LLVMFunctionType* functionType,
                                               int line);
    
    /**
     * Registers an external llvm function with the given type and name
     */
    void registerLLVMExternalFunctionNamedType(std::string name,
                                               const LLVMFunctionType* functionType,
                                               int line);
    
    /**
     * Looks up llvm function type or throws an error if it is not registered
     */
    const LLVMFunctionType* lookupLLVMFunctionNamedType(std::string name, int line);
    
    /**
     * Bind an interface to a controller for injection
     */
    void bindInterfaceToController(std::string interfaceName, std::string controllerName, int line);
    
    /**
     * Returns controller bound to the given interface
     */
    const Controller* getBoundController(const Interface* interface, int line) const;
    
    /**
     * Tells whether there is a controller bound to the given interface
     */
    bool hasBoundController(const Interface* interface) const;
    
    /**
     * Runs code that binds interfaces to controllers
     */
    void bindInterfaces(IRGenerationContext& context) const;
    
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
     * Turns on destrcutor debug mode on that prints out a message when object destructor is called
     */
    void turnDestructorDebugOn();
    
    /**
     * Tells whether the program is compiled with destructor debugging mode on
     */
    bool isDestructorDebugOn() const;
    
    /**
     * Configures compiler to not generate main() function
     */
    void setShouldNotGenerateMain();
    
    /**
     * Tells whether main() function should be generated
     */
    bool shouldGenerateMain() const;
    
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
                                      const void* object);
    
    /**
     * Add a IR composition function callback that will be called after code generation is finished
     */
    void addComposingCallback2Objects(ComposingFunction2Objects callback,
                                      llvm::Function* function,
                                      const void* object1,
                                      const void* object2);
    
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
    
    /**
     * Sets the currently processed method
     */
    void setCurrentMethod(const IMethod* method);
    
    /**
     * Returns the currently processed method
     */
    const IMethod* getCurrentMethod() const;
     
    /**
     * Tells whether compiler is running composing callbacks
     */
    bool isRunningComposingCallbacks() const;
    
    /**
     * Print a compile error message
     */
    void reportError(int line, std::string message) const;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    std::string getLLVMFunctionKeyPrefix(const IType* returnType,
                                         std::vector<const IType*> argumentTypes) const;
    
  };
  
} /* namespace wisey */

#endif /* IRGenerationContext_h */

