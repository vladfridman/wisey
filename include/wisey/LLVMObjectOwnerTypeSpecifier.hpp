//
//  LLVMObjectOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMObjectOwnerTypeSpecifier_h
#define LLVMObjectOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMObjectOwnerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an llvm pointer that points to a wisey object that it owns
   */
  class LLVMObjectOwnerTypeSpecifier : public ITypeSpecifier {
    
  public:
    
    LLVMObjectOwnerTypeSpecifier();
    
    ~LLVMObjectOwnerTypeSpecifier();
    
    const LLVMObjectOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMObjectOwnerTypeSpecifier_h */
