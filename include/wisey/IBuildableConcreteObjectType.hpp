//
//  IBuildableConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IBuildableConcreteObjectType_h
#define IBuildableConcreteObjectType_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
  /**
   * Interface representing a buildable object
   */
  class IBuildableConcreteObjectType : public IConcreteObjectType {
    
  public:
    
    /**
     * Builds an instance of this object and initializes all fields
     */
    virtual llvm::Instruction* build(IRGenerationContext& context,
                                     const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                     int line) const = 0;
    
    /**
     * Declares a function that builds this model
     */
    virtual llvm::Function* declareBuildFunction(IRGenerationContext& context) const = 0;
    
    /**
     * Defines a function that builds this model, composes the function body
     */
    virtual llvm::Function* defineBuildFunction(IRGenerationContext& context) const = 0;
    
    /**
     * Tells whether the object instance is allocated on a memory pool
     */
    virtual bool isPooled() const = 0;
    
    /**
     * Declares a build function for the given buildable object
     */
    static llvm::Function* declareBuildFunctionForObject(IRGenerationContext& context,
                                                         const IBuildableConcreteObjectType*
                                                         object);
    
    /**
     * Defines a build function for the given buildable object, composes the function body
     */
    static llvm::Function* defineBuildFunctionForObject(IRGenerationContext& context,
                                                        const IBuildableConcreteObjectType* object);
    
    /**
     * Returns build function name for the given buildable object
     */
    static std::string getBuildFunctionNameForObject(const IBuildableConcreteObjectType* object);

  private:
    
    
    static void initializeReceivedFieldsForObject(IRGenerationContext& context,
                                                  llvm::Function* buildFunction,
                                                  const IBuildableConcreteObjectType* object,
                                                  llvm::Instruction* malloc);

    static void composeBuildFunctionBody(IRGenerationContext& context,
                                         llvm::Function* buildFunction,
                                         const void* objectType);

  };
  
} /* namespace wisey */

#endif /* IBuildableConcreteObjectType_h */

