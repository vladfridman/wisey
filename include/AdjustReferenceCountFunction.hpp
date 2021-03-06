//
//  AdjustReferenceCountFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCountFunction_h
#define AdjustReferenceCountFunction_h

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Global function that modifies reference count for any types of objects including interfaces
   */
  class AdjustReferenceCountFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call function with given parameters
     */
    static void call(IRGenerationContext& context, llvm::Value* object, int adjustment);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* AdjustReferenceCountFunction_h */

