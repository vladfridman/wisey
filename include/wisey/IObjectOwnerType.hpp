//
//  IObjectOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectOwnerType_h
#define IObjectOwnerType_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  class IObjectType;
  
  /**
   * Represents an IType that is an owner of the model, controller or interface
   */
  class IObjectOwnerType : public IOwnerType {
    
  public:
    
    /**
     * Override method from IType because ObjectOwner llvm type is always a PointerType
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const override = 0;
    
    /**
     * Returns detructor function for this object
     */
    virtual llvm::Function* getDestructorFunction(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IObjectOwnerType_h */

