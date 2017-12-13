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

#include "wisey/IType.hpp"

namespace wisey {
  
class IObjectType;
  
/**
 * Represents an IType that is an owner of the model, controller or interface
 */
class IObjectOwnerType : public IType {
    
public:
  
  /**
   * Returns the owner type for this object
   */
  virtual IObjectType* getObject() const = 0;
  
  /**
   * Override method from IType because ObjectOwner llvm type is always a PointerType
   */
  virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const override = 0;
  
  /**
   * Deallocates memory occupied by this object owner and its field variables
   */
  virtual void free(IRGenerationContext& context, llvm::Value* value) const = 0;

};

} /* namespace wisey */

#endif /* IObjectOwnerType_h */
