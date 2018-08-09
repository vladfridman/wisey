//
//  IConstantExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IConstantExpression_h
#define IConstantExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Interface representing an expression that computes to a constant
   */
  class IConstantExpression : public IExpression {
  };
  
  /**
   * Represents a list of constant expressions
   */
  typedef std::vector<const IConstantExpression*> ConstantExpressionList;

} /* namespace wisey */

#endif /* IConstantExpression_h */
