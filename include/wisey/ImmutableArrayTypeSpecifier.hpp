//
//  ImmutableArrayTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ImmutableArrayTypeSpecifier_h
#define ImmutableArrayTypeSpecifier_h

#include "wisey/ArrayTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/ImmutableArrayType.hpp"

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
