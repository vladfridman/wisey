//
//  IfElseStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IfElseStatement_h
#define IfElseStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {
  
/**
 * Represents an if / else statement
 * if (expression) { statement } else { statement }
 */
class IfElseStatement : public IStatement {
  IExpression& mCondition;
  CompoundStatement& mThenStatement;
  CompoundStatement& mElseStatement;
  
public:
  
  IfElseStatement(IExpression& condition,
                  CompoundStatement& thenStatement,
                  CompoundStatement& elseStatement) :
    mCondition(condition),
    mThenStatement(thenStatement),
    mElseStatement(elseStatement) { }
  
  ~IfElseStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* IfElseStatement_h */
