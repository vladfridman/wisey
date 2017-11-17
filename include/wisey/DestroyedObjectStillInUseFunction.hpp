//
//  DestroyedObjectStillInUseFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DestroyedObjectStillInUseFunction_h
#define DestroyedObjectStillInUseFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Represents the global function that throws DOSIU exception
 */
class DestroyedObjectStillInUseFunction {

public:
  
  /**
   * Returns the llvm function name
   */
  static std::string getName();
  
  /**
   * Defines this function
   */
  static llvm::Function* define(IRGenerationContext& context);
  
  /**
   * Composes function's body
   */
  static void compose(IRGenerationContext& context, const IObjectType* objectType);
  
};
  
} /* namespace wisey */

#endif /* DestroyedObjectStillInUseFunction_h */
