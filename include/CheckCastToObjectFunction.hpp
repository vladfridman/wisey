//
//  CheckForModelFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef CheckForModelFunction_h
#define CheckForModelFunction_h

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Checks that given object can be cast to certain object type, throws an exception if it can't
   */
  class CheckCastToObjectFunction {
    
  public:
    
    /**
     * Returns the function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls to determine if the object can be cast to a model
     */
    static void callCheckCastToModel(IRGenerationContext& context, llvm::Value* object);
    
    /**
     * Calls to determine if the object can be cast to a node
     */
    static void callCheckCastToNode(IRGenerationContext& context, llvm::Value* object);

  private:
    
    static void call(IRGenerationContext& context,
                     llvm::Value* object,
                     llvm::Value* letterCode,
                     llvm::Value* toTypeName);

    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* CheckForModelFunction_h */
