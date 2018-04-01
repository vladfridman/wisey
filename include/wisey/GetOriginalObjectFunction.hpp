//
//  GetOriginalObjectFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef GetOriginalObjectFunction_h
#define GetOriginalObjectFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * A global function that returns the pointer to the object given an interface object
   */
  class GetOriginalObjectFunction {
    
  public:
    
    /**
     * Returns the getOriginalObject function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls getOriginalObject function to get the pointer to the original object's vTable
     */
    static llvm::Value* call(IRGenerationContext& context, llvm::Value* interfacePointer);
  
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static llvm::Value* getUnthunkBy(IRGenerationContext& context, llvm::Value* value);
  };
  
} /* namespace wisey */

#endif /* GetOriginalObjectFunction_h */

