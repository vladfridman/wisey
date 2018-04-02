//
//  LLVMObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMObjectTypeSpecifier_h
#define LLVMObjectTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMObjectType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a llvm pointer that points to a wisey object type specifier
   */
  class LLVMObjectTypeSpecifier : public ITypeSpecifier {
    
  public:
    
    LLVMObjectTypeSpecifier();
    
    ~LLVMObjectTypeSpecifier();
    
    const LLVMObjectType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMObjectTypeSpecifier_h */
