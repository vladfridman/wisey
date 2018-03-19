//
//  LLVMFunctionTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionTypeSpecifier_h
#define LLVMFunctionTypeSpecifier_h

#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm array type with exact dimensions specified
   */
  class LLVMFunctionTypeSpecifier : public ILLVMTypeSpecifier {
    const ILLVMTypeSpecifier* mReturnTypeSpecifier;
    std::vector<const ILLVMTypeSpecifier*> mArgumentTypeSpecifiers;
    
  public:
    
    LLVMFunctionTypeSpecifier(const ILLVMTypeSpecifier* returnTypeSpecifier,
                              std::vector<const ILLVMTypeSpecifier*> argumentTypeSpecifiers);
    
    ~LLVMFunctionTypeSpecifier();
    
    LLVMFunctionType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    std::string toString(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionTypeSpecifier_h */
