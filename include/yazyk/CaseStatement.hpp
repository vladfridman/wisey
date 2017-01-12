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
  bool mIsFallThrough;
  
  CaseStatement(IExpression& expression, Block& block, bool isFallThrough)
    : mExpression(expression), mBlock(block), mIsFallThrough(isFallThrough) { }
  
public:
  
  ~CaseStatement() { }
  
  llvm::ConstantInt* getExpressionValue(IRGenerationContext& context) const;
  
  bool isFallThrough() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  /**
   * Create a CASE statement
   */
  static CaseStatement * newCaseStatement(IExpression& expression, Block& block) {
    return new CaseStatement(expression, block, false);
  }

  /**
   * Create a CASE statement with FALLTHROUGH
   */
  static CaseStatement * newCaseStatementWithFallThrough(IExpression& expression, Block& block) {
    return new CaseStatement(expression, block, true);
  }
};
  
} /* namespace yazyk */

#endif /* CaseStatement_hpp */
