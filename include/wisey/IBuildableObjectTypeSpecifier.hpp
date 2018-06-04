//
//  IBuildableObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IBuildableObjectTypeSpecifier_h
#define IBuildableObjectTypeSpecifier_h

#include <iostream>

#include "wisey/IBuildableObjectType.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Reprensents a type specifier for buildable object types: models and nodes
   */
  class IBuildableObjectTypeSpecifier : public IObjectTypeSpecifier {
    
  public:
    
    virtual const IBuildableObjectType* getType(IRGenerationContext& context) const override = 0;
    
  };
  
} /* namespace wisey */

#endif /* IBuildableObjectTypeSpecifier_h */
