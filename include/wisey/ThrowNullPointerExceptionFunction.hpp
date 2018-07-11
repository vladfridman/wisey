//
//  ThrowNullPointerExceptionFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThrowNullPointerExceptionFunction_h
#define ThrowNullPointerExceptionFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a global function that throws an MNullPointerException
   */
  class ThrowNullPointerExceptionFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls function with a given argument
     */
    static void call(IRGenerationContext& context);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* ThrowNullPointerExceptionFunction_h */
