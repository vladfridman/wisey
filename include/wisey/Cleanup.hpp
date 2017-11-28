//
//  Cleanup.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Cleanup_h
#define Cleanup_h

#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * Methods for generating cleanup landing pad for rethrowing exeptions
 */
class Cleanup {
  
public:
  
  /**
   * Generate landing pad code that is executed once an exception is thrown
   *
   * Returns the cleanup landing pad basic block
   */
  static llvm::BasicBlock* generate(IRGenerationContext& context, int line);

};
  
} /* namespace wisey */

#endif /* Cleanup_h */
