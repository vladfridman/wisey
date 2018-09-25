//
//  IObjectElementDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectElementDefinition_h
#define IObjectElementDefinition_h

#include "IObjectElement.hpp"
#include "IObjectType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a definition of an object element such as a method or a field
   */
  class IObjectElementDefinition {
    
  public:
    
    virtual ~IObjectElementDefinition() { }
    
    /**
     * Returns a represention of an object element: a method or a field
     */
    virtual IObjectElement* define(IRGenerationContext& context,
                                   const IObjectType* objectType) const = 0;
    
    
    /**
     * Methods for determining the type of object element
     */
    virtual bool isConstant() const = 0;
    
    virtual bool isField() const = 0;
    
    virtual bool isMethod() const = 0;
    
    virtual bool isStaticMethod() const = 0;
    
    virtual bool isMethodSignature() const = 0;
    
    virtual bool isLLVMFunction() const = 0;

  };
  
} /* namespace wisey */


#endif /* IObjectElementDefinition_h */

