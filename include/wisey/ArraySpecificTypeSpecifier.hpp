//
//  ArraySpecificTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArraySpecificTypeSpecifier_h
#define ArraySpecificTypeSpecifier_h

#include <list>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a wisey array type with dimensions specified
   */
  class ArraySpecificTypeSpecifier : public ITypeSpecifier {
    ITypeSpecifier* mElementTypeSpecifier;
    std::list<unsigned long> mDimensions;
    
  public:
    
    ArraySpecificTypeSpecifier(ITypeSpecifier* elementTypeSpecifier, std::list<unsigned long> dimensions);
    
    ~ArraySpecificTypeSpecifier();
    
    ArraySpecificType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ArraySpecificTypeSpecifier_h */
