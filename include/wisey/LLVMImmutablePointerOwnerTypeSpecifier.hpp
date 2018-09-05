//
//  LLVMImmutablePointerOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMImmutablePointerOwnerTypeSpecifier_h
#define LLVMImmutablePointerOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMImmutablePointerOwnerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  class LLVMImmutablePointerOwnerType;
  class LLVMImmutablePointerTypeSpecifier;
  
  /**
   * Represents an immutable llvm pointer that points to a native object that it owns
   */
  class LLVMImmutablePointerOwnerTypeSpecifier : public ITypeSpecifier {
    
    const LLVMImmutablePointerTypeSpecifier* mImmutablePointerTypeSpecifier;
    
  public:
    
    LLVMImmutablePointerOwnerTypeSpecifier(const LLVMImmutablePointerTypeSpecifier*
                                           immutablePointerTypeSpecifier);
    
    ~LLVMImmutablePointerOwnerTypeSpecifier();
    
    const LLVMImmutablePointerOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMImmutablePointerOwnerTypeSpecifier_h */
