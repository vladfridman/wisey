//
//  CheckArrayNotReferencedFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef CheckArrayNotReferencedFunction_h
#define CheckArrayNotReferencedFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Checkes that neither array nor any of its sub arrays are referenced, throws RCE otherwise
   */
  class CheckArrayNotReferencedFunction {
    
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
                     llvm::Value* numberOfDimensions);

    /**
     * Call function with the type that contains and array type
     */
    static void callWithArrayType(IRGenerationContext& context,
                                  llvm::Value* array,
                                  const IType* withArrayType);

  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* CheckArrayNotReferencedFunction_h */
