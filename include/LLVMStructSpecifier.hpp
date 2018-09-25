//
//  LLVMStructSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMStructSpecifier_h
#define LLVMStructSpecifier_h

#include "ILLVMPrimitiveType.hpp"
#include "ILLVMTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an llvm struct type
   */
  class LLVMStructSpecifier : public ILLVMTypeSpecifier {
    
    const std::string mName;
    int mLine;
    
  public:
    
    LLVMStructSpecifier(const std::string name, int line);
    
    ~LLVMStructSpecifier();
    
    const ILLVMType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* LLVMStructSpecifier_h */
