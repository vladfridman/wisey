//
//  ILLVMType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ILLVMType_h
#define ILLVMType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Represents an LLVM native type such as i8, i32, or llvm struct or llvm array
   */
  class ILLVMType : public IType {
   
  public:
    
    /**
     * Returns reference type which is the pointer to this type
     */
    virtual const ILLVMType* getPointerType() const = 0;

  };
  
} /* namespace wisey */

#endif /* ILLVMType_h */
