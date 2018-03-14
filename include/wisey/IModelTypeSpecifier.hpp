//
//  IModelTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IModelTypeSpecifier_h
#define IModelTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents model type specifier
   */
  class IModelTypeSpecifier : public IObjectTypeSpecifier {
    
  public:
    
    virtual Model* getType(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IModelTypeSpecifier_h */

