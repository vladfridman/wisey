//
//  Cleanup.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Cleanup_h
#define Cleanup_h

#include "wisey/FinallyBlock.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * Methods for generating cleanup landing pad for rethrowing exeptions
 */
class Cleanup {
  
public:
  
  /**
   * Generate landing pad information for code that might throw an exception
   */
  static void generateCleanupTryCatchInfo(IRGenerationContext& context, std::string label);
  
  /**
   * Generate landing pad code that is executed once an exception is thrown
   */
  static void generateCleanupLandingPad(IRGenerationContext& context, FinallyBlock* finallyBlock);

};
  
} /* namespace wisey */

#endif /* Cleanup_h */
