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
     * Returns build function name
     */
    virtual std::string getBuildFunctionName() const = 0;

  };
  
} /* namespace wisey */

#endif /* IBuildableConcreteObjectType_h */

