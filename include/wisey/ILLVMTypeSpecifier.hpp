//
//  ILLVMTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ILLVMTypeSpecifier_h
#define ILLVMTypeSpecifier_h

#include "wisey/ILLVMType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents an LLVM type specifier
   */
  class ILLVMTypeSpecifier : public ITypeSpecifier {
    
  public:
    
    /**
     * Returns llvm type
     */
    virtual const ILLVMType* getType(IRGenerationContext& context) const override = 0;

  };
  
} /* namespace wisey */

#endif /* ILLVMTypeSpecifier_h */
