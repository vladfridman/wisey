//
//  IntExpressionChecker.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IntExpressionChecker_h
#define IntExpressionChecker_h

#include "IBinaryExpression.hpp"

namespace wisey {

  class IntExpressionChecker {
    
  public:
    
    static void checkTypes(IRGenerationContext& context, const IBinaryExpression* expression);
    
  };
  
} /* namespace wisey */

#endif /* IntExpressionChecker_h */
