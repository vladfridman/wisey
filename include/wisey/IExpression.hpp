//
//  IExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IExpression_h
#define IExpression_h

#include "wisey/IHasType.hpp"
#include "wisey/INode.hpp"

namespace wisey {

/**
 * Interface representing a wisey language expression
 */
class IExpression : public INode, public IHasType {

public:

  /**
   * Release ownership of the object generated by this expression
   */
  virtual void releaseOwnership(IRGenerationContext& context) const = 0;
};
  
/**
 * Represents a list of expressions
 */
typedef std::vector<IExpression*> ExpressionList;

} /* namespace wisey */


#endif /* IExpression_h */
