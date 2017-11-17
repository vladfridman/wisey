//
//  AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCounterForConcreteObjectUnsafelyFunction_h
#define AdjustReferenceCounterForConcreteObjectUnsafelyFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Global function that increments reference count for controllers and nodes
 */
class AdjustReferenceCounterForConcreteObjectUnsafelyFunction {
    
public:
  
  /**
   * Returns the function
   */
  static llvm::Function* get(IRGenerationContext& context);
  
private:
  
  static std::string getName();
  
  static llvm::Function* define(IRGenerationContext& context);
  
  static void compose(IRGenerationContext& context, const IObjectType* objectType);
  
};

} /* namespace wisey */


#endif /* AdjustReferenceCounterForConcreteObjectUnsafelyFunction_h */
