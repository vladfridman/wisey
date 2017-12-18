//
//  IExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IExpression_h
#define IExpression_h

#include <llvm/IR/Value.h>

#include "wisey/IHasType.hpp"
#include "wisey/IPrintable.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

typedef enum IRGenerationFlagEnum {
  IR_GENERATION_NORMAL,
  IR_GENERATION_RELEASE,
} IRGenerationFlag;

class IRGenerationContext;

/**
 * Interface representing a wisey language expression
 */
class IExpression : public IHasType, public IPrintable {

public:

  virtual ~IExpression() { }

  /**
   * Generate LLVM Intermediate Reprentation code
   */
  virtual llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const = 0;

  /**
   * If this expression resolves to a vairable returns the variable otherwise returns NULL
   *
   * This is needed for checking wheather an increment operation can by applied to an expression
   */
  virtual IVariable* getVariable(IRGenerationContext& context,
                                 std::vector<const IExpression*>& arrayIndices) const = 0;

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
