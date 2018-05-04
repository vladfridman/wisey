//
//  CheckArrayIndexFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef CheckArrayIndexFunction_h
#define CheckArrayIndexFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a global function that throws an MArrayIndexOutOfBoundsException if the given
   * index is greater than or equal to the given size.
   */
  class CheckArrayIndexFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls function with given index and array size arguments
     */
    static void call(IRGenerationContext& context, llvm::Value* index, llvm::Value* size, int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* CheckArrayIndexFunction_h */

