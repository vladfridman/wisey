//
//  DecrementReferencesInArrayFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DecrementReferencesInArrayFunction_h
#define DecrementReferencesInArrayFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Function used for decrementing reference counts on all elements in a reference array
   */
  class DecrementReferencesInArrayFunction {
    
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
                     unsigned long size,
                     llvm::Function* decrementFunction);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
  };
  
} /* namespace wisey */

#endif /* DecrementReferencesInArrayFunction_h */

