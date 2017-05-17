//
//  WhileStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef WhileStatement_h
#define WhileStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Represents a while loop statement
 */
class WhileStatement : public IStatement {
  IExpression& mConditionExpression;
  IStatement& mStatement;
  
public:
  
  WhileStatement(IExpression& conditionExpression, IStatement& statement)
    : mConditionExpression(conditionExpression), mStatement(statement) { }
  
  ~WhileStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace wisey */

#endif /* WhileStatement_h */
