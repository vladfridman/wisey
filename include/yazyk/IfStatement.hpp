//
//  IfStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IfStatement_h
#define IfStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {
  
/**
 * Represents an if / else statement
 * if (expression) { statement }
 */
class IfStatement : public IStatement {
  IExpression& mCondition;
  CompoundStatement& mThenStatement;
  
public:
  
  IfStatement(IExpression& condition, CompoundStatement& thenStatement) :
    mCondition(condition), mThenStatement(thenStatement) { }
  
  ~IfStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* IfStatement_h */
