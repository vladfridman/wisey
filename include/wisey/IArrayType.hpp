//
//  IArrayType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IArrayType_h
#define IArrayType_h

#include <llvm/IR/Instructions.h>

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Represents an array type that could either be of reference or owner type kind
   */
  class IArrayType : public IType {
    
  public:
    
    /**
     * Return array base type
     */
    virtual const IType* getBaseType() const = 0;
    
    /**
     * Return array size
     */
    virtual unsigned long getSize() const = 0;
    
    /**
     * Returns single array element type
     */
    virtual const IType* getScalarType() const = 0;
    
  };
  
}
#endif /* IArrayType_h */
