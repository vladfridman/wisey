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

#endif /* DestroyedObjectStillInUseFunction_h */
