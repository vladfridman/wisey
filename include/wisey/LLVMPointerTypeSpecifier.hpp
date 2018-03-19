//
//  LLVMPointerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPointerTypeSpecifier_h
#define LLVMPointerTypeSpecifier_h

#include "wisey/ILLVMTypeSpecifier.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a llvm pointer type specifier such as ::llvm::i8*
   */
  class LLVMPointerTypeSpecifier : public ILLVMTypeSpecifier {
    
    const ILLVMTypeSpecifier* mBaseTypeSpecifier;
    
  public:
    
    LLVMPointerTypeSpecifier(const ILLVMTypeSpecifier* baseTypeSpecifier);
    
    ~LLVMPointerTypeSpecifier();
    
    const ILLVMType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  };
  
} /* namespace wisey */

#endif /* LLVMPointerTypeSpecifier_h */
