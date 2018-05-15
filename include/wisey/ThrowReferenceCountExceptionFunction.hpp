//
//  ThrowReferenceCountExceptionFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ThrowReferenceCountExceptionFunction_h
#define ThrowReferenceCountExceptionFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents the global function that throws Reference Count Exception
   */
  class ThrowReferenceCountExceptionFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call the function with given arguments
     */
    static void call(IRGenerationContext& context,
                     llvm::Value* referenceCount,
                     llvm::Value* namePointer);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* ThrowReferenceCountExceptionFunction_h */

