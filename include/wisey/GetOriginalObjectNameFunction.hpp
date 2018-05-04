//
//  GetOriginalObjectNameFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef GetOriginalObjectNameFunction_h
#define GetOriginalObjectNameFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Returns pointer to the string literal containing the given object's name
   */
  class GetOriginalObjectNameFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls the function with given pointer to object that could be an interface or another object
     */
    static llvm::Value* call(IRGenerationContext& context, llvm::Value* object, int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* GetOriginalObjectNameFunction_h */
