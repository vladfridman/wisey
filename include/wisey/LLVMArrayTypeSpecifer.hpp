//
//  LLVMArrayTypeSpecifer.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMArrayTypeSpecifer_h
#define LLVMArrayTypeSpecifer_h

#include <list>

#include "wisey/LLVMArrayType.hpp"
#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents an llvm array type with exact dimensions specified
   */
  class LLVMArrayTypeSpecifer : public ILLVMTypeSpecifier {
    const ITypeSpecifier* mElementTypeSpecifier;
    std::list<unsigned long> mDimensions;
    
  public:
    
    LLVMArrayTypeSpecifer(const ITypeSpecifier* elementTypeSpecifier,
                          std::list<unsigned long> dimensions);
    
    ~LLVMArrayTypeSpecifer();
    
    LLVMArrayType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMArrayTypeSpecifer_h */
