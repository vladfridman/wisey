//
//  IHasType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IHasType_h
#define IHasType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Interface representing a typed entity
   */
  class IHasType {
    
  public:
    
    virtual ~IHasType() { }
    
    /**
     * Returns type
     */
    virtual const IType* getType(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IHasType_h */

