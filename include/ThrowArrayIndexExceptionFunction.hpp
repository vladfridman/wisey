//
//  ThrowArrayIndexExceptionFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThrowArrayIndexExceptionFunction_h
#define ThrowArrayIndexExceptionFunction_h

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a global function that throws an MArrayIndexOutOfBoundsException
   */
  class ThrowArrayIndexExceptionFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls function with a given argument
     */
    static void call(IRGenerationContext& context,
                     llvm::Value* size,
                     llvm::Value* index,
                     int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* ThrowArrayIndexExceptionFunction_h */
