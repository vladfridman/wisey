//
//  PrimitiveTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef PrimitiveTypeSpecifier_h
#define PrimitiveTypeSpecifier_h

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a wisey primitive type
   */
  class PrimitiveTypeSpecifier : public ITypeSpecifier {

    const IType* mType;
    int mLine;

  public:
    
    PrimitiveTypeSpecifier(const IType* type, int line);
    
    ~PrimitiveTypeSpecifier();
    
    const IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* PrimitiveTypeSpecifier_h */

