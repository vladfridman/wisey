//
//  AdjustReferenceCounterForConcreteObjectSafelyFunction.h
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCounterForConcreteObjectSafelyFunction_h
#define AdjustReferenceCounterForConcreteObjectSafelyFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Global function that increments reference count for models in a thread safe manner
 */
class AdjustReferenceCounterForConcreteObjectSafelyFunction {
    
public:
  
  /**
   * Returns the function
   */
  static llvm::Function* get(IRGenerationContext& context);
  
  /**
   * Call function with given parameters
   */
  static void call(IRGenerationContext& context, llvm::Value* object, int adjustment);
  
private:
  
  static std::string getName();
  
  static llvm::Function* define(IRGenerationContext& context);
  
  static void compose(IRGenerationContext& context, llvm::Function* function);
  
};

} /* namespace wisey */

#endif /* AdjustReferenceCounterForConcreteObjectSafelyFunction_h */
