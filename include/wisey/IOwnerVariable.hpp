//
//  IOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IOwnerVariable_h
#define IOwnerVariable_h

#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a variable of owner type
   */
  class IOwnerVariable : public IVariable {
    
  public:
    
    /**
     * Sets variable to null
     */
    virtual void setToNull(IRGenerationContext& context, int line) = 0;
    
    /**
     * Free any allocated space associated with this variable
     */
    virtual void free(IRGenerationContext& context, int line) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IOwnerVariable_h */

