//
//  ArrayOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayOwnerTypeSpecifier_h
#define ArrayOwnerTypeSpecifier_h

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a wisey array type owner reference
   */
  class ArrayOwnerTypeSpecifier : public ITypeSpecifier {
    ArrayTypeSpecifier* mArrayTypeSpecifier;
    
  public:
    
    ArrayOwnerTypeSpecifier(ArrayTypeSpecifier* arrayTypeSpecifier);
    
    ~ArrayOwnerTypeSpecifier();
    
    const ArrayOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */


#endif /* ArrayOwnerTypeSpecifier_h */
