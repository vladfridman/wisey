//
//  Optimizer.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef Optimizer_h
#define Optimizer_h

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents IR code optimizer
   */
  class Optimizer {
    
  public:
    
    /**
     * Optimizies IR code stored in context
     */
    static void optimize(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* Optimizer_h */
