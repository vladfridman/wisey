//
//  ITypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ITypeSpecifier_h
#define ITypeSpecifier_h

#include "wisey/IHasType.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a type specifier that could either be a primitive or an object type
   */
  class ITypeSpecifier : public IHasType, public IPrintable {
    
  public:

    /**
     * Returns the line number where this type specifier is defined
     */
    virtual int getLine() const = 0;
    
  };
  
} /* namespace wisey */

#endif /* ITypeSpecifier_h */

