//
//  LLVMPointerOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPointerOwnerTypeSpecifier_h
#define LLVMPointerOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  class LLVMPointerTypeSpecifier;
  
  /**
   * Represents an llvm pointer that points to a native object that it owns
   */
  class LLVMPointerOwnerTypeSpecifier : public ITypeSpecifier {
    
    const LLVMPointerTypeSpecifier* mPointerTypeSpecifier;
    
  public:
    
    LLVMPointerOwnerTypeSpecifier(const LLVMPointerTypeSpecifier* pointerTypeSpecifier);
    
    ~LLVMPointerOwnerTypeSpecifier();
    
    const LLVMPointerOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* LLVMPointerOwnerTypeSpecifier_h */
