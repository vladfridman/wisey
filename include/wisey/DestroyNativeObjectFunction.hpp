//
//  DestroyNativeObjectFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DestroyNativeObjectFunction_h
#define DestroyNativeObjectFunction_h

namespace wisey {
  
  class IRGenerationContext;
  class LLVMFunctionType;
  
  /**
   * Function for destroying native objects. Checks if the object is not null and calls free on it.
   */
  class DestroyNativeObjectFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call function with given parameters
     */
    static void call(IRGenerationContext& context, llvm::Value* objectReference, int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* DestroyNativeObjectFunction_h */
