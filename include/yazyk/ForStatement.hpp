//
//  ForStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ForStatement_h
#define ForStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a while loop statement
 */
class ForStatement : public IStatement {
  IStatement& mStartStatement;
  IStatement& mConditionStatement;
  IExpression& mIncrementExpression;
  IStatement& mBodyStatement;
  
public:
  
  ForStatement(IStatement& startStatement,
               IStatement& conditionStatement,
               IExpression& incrementExpression,
               IStatement& bodyStatement) :
    mStartStatement(startStatement),
    mConditionStatement(conditionStatement),
    mIncrementExpression(incrementExpression),
    mBodyStatement(bodyStatement) { }
  
  ~ForStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* ForStatement_h */
