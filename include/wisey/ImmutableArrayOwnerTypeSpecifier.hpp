//
//  ImmutableArrayOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ImmutableArrayOwnerTypeSpecifier_h
#define ImmutableArrayOwnerTypeSpecifier_h

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/ImmutableArrayTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a wisey immutable array type owner reference
   */
  class ImmutableArrayOwnerTypeSpecifier : public ITypeSpecifier {
    ImmutableArrayTypeSpecifier* mImmutableArrayTypeSpecifier;
    
  public:
    
    ImmutableArrayOwnerTypeSpecifier(ImmutableArrayTypeSpecifier* immutableArrayTypeSpecifier);
    
    ~ImmutableArrayOwnerTypeSpecifier();
    
    const ImmutableArrayOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
  };
  
} /* namespace wisey */

#endif /* ImmutableArrayOwnerTypeSpecifier_h */
