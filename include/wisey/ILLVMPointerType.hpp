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
  
  class LLVMObjectOwnerType;
  
  /**
   * Represents a pointer type to an llvm type
   */
  class ILLVMPointerType : public IReferenceType {
    
  public:
    
    /**
     * Return corresponding LLVM type
     */
    virtual llvm::PointerType* getLLVMType(IRGenerationContext& context) const = 0;

  };
  
} /* namespace wisey */

#endif /* ILLVMPointerType_h */
