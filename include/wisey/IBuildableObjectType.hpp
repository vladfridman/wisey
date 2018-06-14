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
     * Generates builder or allocator argument values and reports corresponding errors
     */
    virtual void generateCreationArguments(IRGenerationContext& context,
                                           const ObjectBuilderArgumentList& objectBuilderArguments,
                                           std::vector<llvm::Value*>& argumentValues,
                                           int line) const = 0;
    
    /**
     * Returns a list of received fields
     */
    virtual std::vector<IField*> getReceivedFields() const = 0;

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

