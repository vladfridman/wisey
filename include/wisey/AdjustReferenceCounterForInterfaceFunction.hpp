//
//  AdjustReferenceCounterForInterfaceFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustReferenceCounterForInterfaceFunction_h
#define AdjustReferenceCounterForInterfaceFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Global function that increments reference count for objects of type interface
 */
class AdjustReferenceCounterForInterfaceFunction {
    
public:
  
  /**
   * Returns the function
   */
  static llvm::Function* get(IRGenerationContext& context);
  
private:
  
  static std::string getName();
  
  static llvm::Function* define(IRGenerationContext& context);
  
  static void compose(IRGenerationContext& context,
                      llvm::Function* function,
                      const IObjectType* objectType);
  
};

} /* namespace wisey */

#endif /* AdjustReferenceCounterForInterfaceFunction_h */
