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
#include "wisey/IPrintable.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Interface representing a wisey language expression
 */
class IExpression : public INode, public IHasType, public IPrintable {

public:

  /**
   * If this expression resolves to a vairable returns the variable otherwise returns NULL
   *
   * This is needed for checking wheather an increment operation can by applied to an expression
   */
  virtual IVariable* getVariable(IRGenerationContext& context) const = 0;
  
  /**
   * Release ownership of the object generated by this expression
   */
  virtual void releaseOwnership(IRGenerationContext& context) const = 0;
  
  /**
   * Tells whether the value returned by expression exists in the outside caller's method scope
   */
  virtual bool existsInOuterScope(IRGenerationContext& context) const = 0;

  /**
   * Tells whether this expression returns a constant
   */
  virtual bool isConstant() const = 0;

};
  
/**
 * Represents a list of expressions
 */
typedef std::vector<IExpression*> ExpressionList;

} /* namespace wisey */


#endif /* IExpression_h */
