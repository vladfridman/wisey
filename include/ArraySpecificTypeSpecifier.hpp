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

#include "ArraySpecificType.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IType.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a wisey array type with dimensions specified
   */
  class ArraySpecificTypeSpecifier : public ITypeSpecifier {
    const ITypeSpecifier* mElementTypeSpecifier;
    std::list<const IExpression*> mDimensions;
    int mLine;
    
  public:
    
    ArraySpecificTypeSpecifier(const ITypeSpecifier* elementTypeSpecifier,
                               std::list<const IExpression*> dimensions,
                               int line);
    
    ~ArraySpecificTypeSpecifier();
    
    ArraySpecificType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ArraySpecificTypeSpecifier_h */
