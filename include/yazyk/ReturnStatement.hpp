//
//  ReturnStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ReturnStatement_h
#define ReturnStatement_h

#include <exception>

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents a return statement that returns an expression
 */
class ReturnStatement : public IStatement {
public:
  IExpression& expression;
  
  ReturnStatement(IExpression& expression) : expression(expression) { }
  ~ReturnStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

#endif /* ReturnStatement_h */

}