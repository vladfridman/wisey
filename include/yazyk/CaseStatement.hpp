//
//  CaseStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CaseStatement_h
#define CaseStatement_h

#include <llvm/IR/Constants.h>

#include "yazyk/Block.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {
  
/**
 * Represents a CASE statement inside a SWITCH statement
 */
class CaseStatement : public IStatement {
  IExpression& mExpression;
  Block& mBlock;
  
public:
  
  CaseStatement(IExpression& expression, Block& block)
  : mExpression(expression), mBlock(block) { }
  
  ~CaseStatement() { }
  
  llvm::ConstantInt* getExpressionValue(IRGenerationContext& context) const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* CaseStatement_hpp */
