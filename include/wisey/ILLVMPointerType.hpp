//
//  ILLVMPointerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ILLVMPointerType_h
#define ILLVMPointerType_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/IReferenceType.hpp"

namespace wisey {
  
  class LLVMPointerOwnerType;
  
  /**
   * Represents a pointer type to an llvm type
   */
  class ILLVMPointerType : public IReferenceType {
    
  public:
    
    /**
     * Returns owner type that represents a pointer that owns the object it references
     */
    virtual const LLVMPointerOwnerType* getOwner() const = 0;
    
    /**
     * Return corresponding LLVM type
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const = 0;

  };
  
} /* namespace wisey */

#endif /* ILLVMPointerType_h */
