//
//  ILLVMPrimitiveType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ILLVMPrimitiveType_h
#define ILLVMPrimitiveType_h

#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/ILLVMType.hpp"

namespace wisey {
  
  /**
   * Represents an LLVM primitive type such as i8, i32, etc.
   */
  class ILLVMPrimitiveType : public ILLVMType {
    
  public:
    
    /**
     * Creates a type specifier for this type
     */
    virtual const ILLVMTypeSpecifier* newTypeSpecifier(int line) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* ILLVMPrimitiveType_h */
