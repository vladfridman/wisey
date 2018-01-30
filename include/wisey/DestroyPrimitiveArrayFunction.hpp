//
//  DestroyPrimitiveArrayFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DestroyPrimitiveArrayFunction_h
#define DestroyPrimitiveArrayFunction_h

#include "wisey/IPrimitiveType.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Global function used for destroying elements in an array where elements are of primitive type
   */
  class DestroyPrimitiveArrayFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call function with given parameters
     */
    static void call(IRGenerationContext& context,
                     llvm::Value* array,
                     unsigned long numberOfDimensions,
                     const IPrimitiveType* primitiveType);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
  };
  
} /* namespace wisey */


#endif /* DestroyPrimitiveArrayFunction_h */
