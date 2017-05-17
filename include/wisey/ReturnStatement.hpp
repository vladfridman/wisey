//
//  ReturnStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ReturnStatement_h
#define ReturnStatement_h

#include "wisey/IExpression.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Represents a return statement that returns an expression
 */
class ReturnStatement : public IStatement {
public:
  IExpression& mExpression;
  
  ReturnStatement(IExpression& expression) : mExpression(expression) { }
  ~ReturnStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace wisey */

#endif /* ReturnStatement_h */
