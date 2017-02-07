//
//  IExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IExpression_h
#define IExpression_h

#include "yazyk/IHasType.hpp"
#include "yazyk/INode.hpp"

namespace yazyk {

/**
 * Interface representing a yazyk language expression
 */
class IExpression : public INode, public IHasType {
};
  
/**
 * Represents a list of expressions
 */
typedef std::vector<IExpression*> ExpressionList;

} /* namespace yazyk */


#endif /* IExpression_h */
