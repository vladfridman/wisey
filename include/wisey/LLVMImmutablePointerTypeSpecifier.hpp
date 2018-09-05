//
//  LLVMImmutablePointerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMImmutablePointerTypeSpecifier_h
#define LLVMImmutablePointerTypeSpecifier_h

#include "wisey/LLVMImmutablePointerType.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an immutable llvm pointer type specifier such as ::llvm::i8::immutable
   */
  class LLVMImmutablePointerTypeSpecifier : public ILLVMTypeSpecifier {
    
    const LLVMPointerTypeSpecifier* mPointerTypeSpecifier;
     
  public:
    
    LLVMImmutablePointerTypeSpecifier(const LLVMPointerTypeSpecifier* pointerTypeSpecifier);
    
    ~LLVMImmutablePointerTypeSpecifier();
    
    const LLVMImmutablePointerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMImmutablePointerTypeSpecifier_h */
