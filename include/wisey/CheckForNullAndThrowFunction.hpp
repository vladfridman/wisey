//
//  CheckForNullAndThrowFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CheckForNullAndThrowFunction_h
#define CheckForNullAndThrowFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a global function that throws an MNullPointerException if the given parameter is null
   */
  class CheckForNullAndThrowFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls function with a given argument
     */
    static void call(IRGenerationContext& context, llvm::Value* value, int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);

  };
  
} /* namespace wisey */

#endif /* CheckForNullAndThrowFunction_h */

