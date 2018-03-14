//
//  IReferenceType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IReferenceType_h
#define IReferenceType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Represents an IType that is a reference to an object or an array
   */
  class IReferenceType : public IType {
    
  public:
    
    /**
     * Increments reference counter for this object
     */
    virtual void incrementReferenceCount(IRGenerationContext& context,
                                         llvm::Value* object) const = 0;
    
    /**
     * Decrements reference counter for this object
     */
    virtual void decrementReferenceCount(IRGenerationContext& context,
                                         llvm::Value* object) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IReferenceType_h */

