//
//  LLVMStructSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMStructSpecifier_h
#define LLVMStructSpecifier_h

#include "wisey/ILLVMPrimitiveType.hpp"
#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an llvm struct type
   */
  class LLVMStructSpecifier : public ILLVMTypeSpecifier {
    
    const std::string mName;
    
  public:
    
    LLVMStructSpecifier(const std::string name);
    
    ~LLVMStructSpecifier();
    
    const IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMStructSpecifier_h */
