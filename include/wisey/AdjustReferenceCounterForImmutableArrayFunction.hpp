//
//  AdjustReferenceCounterForImmutableArrayFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCounterForImmutableArrayFunction_h
#define AdjustReferenceCounterForImmutableArrayFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Global function that increments reference count for immutable array objects
   */
  class AdjustReferenceCounterForImmutableArrayFunction {
    
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

#endif /* AdjustReferenceCounterForImmutableArrayFunction_h */
