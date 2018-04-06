//
//  IOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IOwnerType_h
#define IOwnerType_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/IType.hpp"

namespace wisey {
  
  class IReferenceType;
  
  /**
   * Represents an IType that is an owner reference
   */
  class IOwnerType : public IType {
    
  public:
    
    /**
     * Return corresponding LLVM type
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const = 0;

    /**
     * Deallocates memory occupied by this object owner and its field variables
     */
    virtual void free(IRGenerationContext& context, llvm::Value* value, int line) const = 0;
    
    /**
     * Returns IReferenceType for this owner type
     */
    virtual const IReferenceType* getReference() const = 0;

  };
  
} /* namespace wisey */

#endif /* IOwnerType_h */

