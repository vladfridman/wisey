//
//  IOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IOwnerType_h
#define IOwnerType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Represents an IType that is an owner reference
   */
  class IOwnerType {
    
  public:
    
    virtual ~IOwnerType() { }
    
    /**
     * Deallocates memory occupied by this object owner and its field variables
     */
    virtual void free(IRGenerationContext& context, llvm::Value* value) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IOwnerType_h */
