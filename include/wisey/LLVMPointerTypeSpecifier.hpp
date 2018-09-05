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
#include "wisey/LLVMPointerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a llvm pointer type specifier such as ::llvm::i8
   */
  class LLVMPointerTypeSpecifier : public ILLVMTypeSpecifier {
    
    const ILLVMTypeSpecifier* mBaseTypeSpecifier;
    int mLine;
    
  public:
    
    LLVMPointerTypeSpecifier(const ILLVMTypeSpecifier* baseTypeSpecifier, int line);
    
    ~LLVMPointerTypeSpecifier();
    
    const LLVMPointerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* LLVMPointerTypeSpecifier_h */
