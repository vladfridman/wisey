//
//  IReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IReferenceVariable_h
#define IReferenceVariable_h

#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a variable of reference type
   */
  class IReferenceVariable : public IVariable {
    
  public:
    
    /**
     * Decrements reference counter for the object pointed to by this variable
     */
    virtual void decrementReferenceCounter(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IReferenceVariable_h */

