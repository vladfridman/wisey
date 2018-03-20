//
//  IMethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodDeclaration_h
#define IMethodDeclaration_h

#include "wisey/IMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents a method declaration that could either be static or non-static
   */
  class IMethodDeclaration : public IObjectElementDefinition {
    
  public:
    
    virtual ~IMethodDeclaration() { }
    
    /**
     * Helper function for creating method argument list for different types of methods
     */
    static std::vector<MethodArgument*> createArgumnetList(IRGenerationContext& context,
                                                           VariableList argumentDefinitions);
    
    /**
     * Helper function for creating exception type list for different types of methods
     */
    static std::vector<const Model*> createExceptionList(IRGenerationContext& context,
                                                         std::vector<IModelTypeSpecifier*>
                                                         thrownExceptions);
    
  };
  
} /* namespace wisey */

#endif /* IMethodDeclaration_h */

