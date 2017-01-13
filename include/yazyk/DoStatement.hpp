//
//  DoStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DoWhileStatement_h
#define DoWhileStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a while loop statement
 */
class DoStatement : public IStatement {
  IStatement& mStatement;
  IExpression& mConditionExpression;
  
public:
  
  DoStatement(IStatement& statement, IExpression& conditionExpression)
    : mStatement(statement), mConditionExpression(conditionExpression) { }
  
  ~DoStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* DoStatement_h */
