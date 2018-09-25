//
//  ImmutableArrayTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ImmutableArrayTypeSpecifier_h
#define ImmutableArrayTypeSpecifier_h

#include "ArrayTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "IType.hpp"
#include "ITypeSpecifier.hpp"
#include "ImmutableArrayType.hpp"

namespace wisey {
  
  /**
   * Represents a wisey immutable array type
   */
  class ImmutableArrayTypeSpecifier : public ITypeSpecifier {
    const ArrayTypeSpecifier* mArrayTypeSpecifier;
    
  public:
    
    ImmutableArrayTypeSpecifier(const ArrayTypeSpecifier* arrayTypeSpecifier);
    
    ~ImmutableArrayTypeSpecifier();
    
    const ImmutableArrayType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
  };
  
} /* namespace wisey */

#endif /* ImmutableArrayTypeSpecifier_h */
