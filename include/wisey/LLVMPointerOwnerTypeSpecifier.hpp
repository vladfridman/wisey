//
//  LLVMPointerOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPointerOwnerTypeSpecifier_h
#define LLVMPointerOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an llvm pointer that owns the object it references
   */
  class LLVMPointerOwnerTypeSpecifier : public ITypeSpecifier {
    
    const LLVMPointerTypeSpecifier* mBaseTypeSpecifier;
    
  public:
    
    LLVMPointerOwnerTypeSpecifier(const LLVMPointerTypeSpecifier* baseTypeSpecifier);
    
    ~LLVMPointerOwnerTypeSpecifier();
    
    const LLVMPointerOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMPointerOwnerTypeSpecifier_h */
