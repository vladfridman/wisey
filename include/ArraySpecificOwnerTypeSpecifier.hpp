//
//  ArraySpecificOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArraySpecificOwnerTypeSpecifier_h
#define ArraySpecificOwnerTypeSpecifier_h

#include <list>

#include "ArraySpecificOwnerType.hpp"
#include "ArraySpecificTypeSpecifier.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IType.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents an array owner type with dimensions specified
   */
  class ArraySpecificOwnerTypeSpecifier : public ITypeSpecifier {
    ArraySpecificTypeSpecifier* mArraySpecificTypeSpecifier;
    
  public:
    
    ArraySpecificOwnerTypeSpecifier(ArraySpecificTypeSpecifier* arraySpecificTypeSpecifier);
    
    ~ArraySpecificOwnerTypeSpecifier();
    
    const ArraySpecificOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ArraySpecificOwnerTypeSpecifier_h */
