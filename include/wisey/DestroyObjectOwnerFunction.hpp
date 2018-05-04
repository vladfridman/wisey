//
//  DestroyObjectOwnerFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DestroyObjectOwnerFunction_h
#define DestroyObjectOwnerFunction_h

namespace wisey {
  
  class IRGenerationContext;
  class LLVMFunctionType;
  
  /**
   * Function for destroying owner references of any kind except arrays
   */
  class DestroyObjectOwnerFunction {
    
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

#endif /* DestroyObjectOwnerFunction_h */
