//
//  ArrayOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayOwnerTypeSpecifier_h
#define ArrayOwnerTypeSpecifier_h

#include "ArrayOwnerType.hpp"
#include "ArrayTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "IType.hpp"
#include "ITypeSpecifier.hpp"

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
    
    int getLine() const override;
    
  };
  
} /* namespace wisey */


#endif /* ArrayOwnerTypeSpecifier_h */

