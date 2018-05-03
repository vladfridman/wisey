//
//  IObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectTypeSpecifier_h
#define IObjectTypeSpecifier_h

#include <iostream>

#include "wisey/IObjectType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Reprensents a type specifier for object types such as controller, model or interface
   */
  class IObjectTypeSpecifier : public ITypeSpecifier {
    
  public:
    
    /**
     * Returns the short object type name without the package prefix
     */
    virtual std::string getShortName() const = 0;
    
    /**
     * Returns the package expression and clears it in this type specifier
     */
    virtual IExpression* takePackage() = 0;
    
    /**
     * Returns the full name with the package prefix
     */
    virtual std::string getName(IRGenerationContext& context) const = 0;
    
    virtual const IObjectType* getType(IRGenerationContext& context) const override = 0;
    
    /**
     * Given the package and the short name return the full name
     */
    static std::string getFullName(IRGenerationContext& context,
                                   std::string shortName,
                                   IExpression* packageExpression,
                                   int line);
    
  };
  
} /* namespace wisey */

#endif /* IObjectTypeSpecifier_h */

