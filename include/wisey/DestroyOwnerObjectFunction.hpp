//
//  DestroyOwnerObjectFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DestroyOwnerObjectFunction_h
#define DestroyOwnerObjectFunction_h

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Function for destroying owner objects of any kind except arrays
   */
  class DestroyOwnerObjectFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call function with given parameters
     */
    static void call(IRGenerationContext& context, llvm::Value* objectReference);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
  };
  
} /* namespace wisey */

#endif /* DestroyOwnerObjectFunction_h */
