//
//  LLVMPrimitiveTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPrimitiveTypeSpecifier_h
#define LLVMPrimitiveTypeSpecifier_h

#include "wisey/ILLVMPrimitiveType.hpp"
#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an llvm primitive type
   */
  class LLVMPrimitiveTypeSpecifier : public ILLVMTypeSpecifier {
    
    const ILLVMPrimitiveType* mType;
    
  public:
    
    LLVMPrimitiveTypeSpecifier(const ILLVMPrimitiveType* type) : mType(type) { }
    
    ~LLVMPrimitiveTypeSpecifier() { }
    
    const ILLVMPrimitiveType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMPrimitiveTypeSpecifier_h */
