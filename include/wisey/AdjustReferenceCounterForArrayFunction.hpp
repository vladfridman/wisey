//
//  AdjustReferenceCounterForArrayFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCounterForArrayFunction_h
#define AdjustReferenceCounterForArrayFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Global function that increments reference count for array objects
   */
  class AdjustReferenceCounterForArrayFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call function with given parameters
     */
    static void call(IRGenerationContext& context, llvm::Value* array, int adjustment, int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* AdjustReferenceCounterForArrayFunction_h */
