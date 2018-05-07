//
//  IsObjectFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IsObjectFunction_h
#define IsObjectFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Determines whether the given object is a cetetain type of object, works with interfaces
   */
  class IsObjectFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Call to determine if the object is a model
     */
    static llvm::Value* callIsModel(IRGenerationContext& context, llvm::Value* object);
    
    /**
     * Call to determine if the object is a thread controller
     */
    static llvm::Value* callIsThread(IRGenerationContext& context, llvm::Value* object);

    /**
     * Call to determine if the object's name starts with the given letter
     */
    static llvm::Value* call(IRGenerationContext& context,
                             llvm::Value* object,
                             llvm::Value* letter);

  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* IsObjectFunction_h */
