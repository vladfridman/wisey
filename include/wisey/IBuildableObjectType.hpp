//
//  IBuildableObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IBuildableObjectType_h
#define IBuildableObjectType_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
  /**
   * Interface representing a buildable object
   */
  class IBuildableObjectType : public IConcreteObjectType {
    
  public:
    
    /**
     * Builds an instance of this object and initializes all fields
     */
    virtual llvm::Instruction* build(IRGenerationContext& context,
                                     const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                     int line) const = 0;
    
    /**
     * Allocates an instance of this object on a given memory pool
     */
    virtual llvm::Instruction* allocate(IRGenerationContext& context,
                                        const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                        IExpression* memoryPool,
                                        int line) const = 0;

    /**
     * Declares a function that builds this object
     */
    virtual llvm::Function* declareBuildFunction(IRGenerationContext& context) const = 0;

    /**
     * Declares a function that allocates this object on a memory pool
     */
    virtual llvm::Function* declareAllocateFunction(IRGenerationContext& context) const = 0;

    /**
     * Defines a function that builds this object, composes the function body
     */
    virtual llvm::Function* defineBuildFunction(IRGenerationContext& context) const = 0;

    /**
     * Defines a function that allocates this object on memory pool, composes the function body
     */
    virtual llvm::Function* defineAllocateFunction(IRGenerationContext& context) const = 0;

    /**
     * Declares a build function for the given buildable object
     */
    static llvm::Function* declareBuildFunctionForObject(IRGenerationContext& context,
                                                         const IBuildableObjectType*
                                                         object);

    /**
     * Declares a allocate function for the given buildable object
     */
    static llvm::Function* declareAllocateFunctionForObject(IRGenerationContext& context,
                                                            const IBuildableObjectType*
                                                            object);

    /**
     * Defines a build function for the given buildable object, composes the function body
     */
    static llvm::Function* defineBuildFunctionForObject(IRGenerationContext& context,
                                                        const IBuildableObjectType* object);

    /**
     * Defines allocate function for the given allocatable object, composes the function body
     */
    static llvm::Function* defineAllocateFunctionForObject(IRGenerationContext& context,
                                                           const IBuildableObjectType* object);

    /**
     * Returns build function name for the given buildable object
     */
    static std::string getBuildFunctionNameForObject(const IBuildableObjectType* object);

    /**
     * Returns allocate function name for the given allocatable object
     */
    static std::string getAllocateFunctionNameForObject(const IBuildableObjectType* object);

  private:
    
    
    static void initializeReceivedFieldsForObject(IRGenerationContext& context,
                                                  llvm::Function::arg_iterator functionArguments,
                                                  const IBuildableObjectType* object,
                                                  llvm::Instruction* malloc);

    static void composeBuildFunctionBody(IRGenerationContext& context,
                                         llvm::Function* buildFunction,
                                         const void* objectType);

    static void composeAllocateFunctionBody(IRGenerationContext& context,
                                            llvm::Function* buildFunction,
                                            const void* objectType);

  };
  
} /* namespace wisey */

#endif /* IBuildableObjectType_h */

