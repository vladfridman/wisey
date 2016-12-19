//
//  ExpressionStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/19/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ExpressionStatement_h
#define ExpressionStatement_h

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents a statement
 */
class ExpressionStatement : public IStatement {
  IExpression& expression;
  
public:
  ExpressionStatement(IExpression& expression) : expression(expression) { }

  ~ExpressionStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
} /* namespace yazyk */

#endif /* ExpressionStatement_h */
