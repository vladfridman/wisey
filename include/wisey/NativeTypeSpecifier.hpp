//
//  NativeTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef NativeTypeSpecifier_h
#define NativeTypeSpecifier_h

#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a native type specifier
   */
  class NativeTypeSpecifier : public ITypeSpecifier {
    IType* mType;
    
  public:
    
    NativeTypeSpecifier(IType* type);
    
    ~NativeTypeSpecifier();
    
    IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* NativeTypeSpecifier_h */
