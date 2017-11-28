//
//  IfElseStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IfElseStatement_h
#define IfElseStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/CompoundStatement.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * Represents an if / else statement
 * if (expression) { statement } else { statement }
 */
class IfElseStatement : public IStatement {
  IExpression* mCondition;
  CompoundStatement* mThenStatement;
  IStatement* mElseStatement;
  
public:
  
  IfElseStatement(IExpression* condition,
                  CompoundStatement* thenStatement,
                  IStatement* elseStatement);
  
  ~IfElseStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* IfElseStatement_h */
