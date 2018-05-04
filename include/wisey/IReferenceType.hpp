//
//  IReferenceType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IReferenceType_h
#define IReferenceType_h

#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents an IType that is a reference to an object or an array
   */
  class IReferenceType : public IType {
    
  public:
    
    /**
     * Return corresponding LLVM type
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const = 0;

    /**
     * Increments reference counter for this object
     */
    virtual void incrementReferenceCount(IRGenerationContext& context,
                                         llvm::Value* object,
                                         int line) const = 0;
    
    /**
     * Decrements reference counter for this object
     */
    virtual void decrementReferenceCount(IRGenerationContext& context,
                                         llvm::Value* object,
                                         int line) const = 0;
    
    /**
     * Returns the owner type for this object
     */
    virtual const IOwnerType* getOwner() const = 0;

  };
  
} /* namespace wisey */

#endif /* IReferenceType_h */

