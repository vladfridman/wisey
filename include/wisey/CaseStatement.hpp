//
//  CaseStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CaseStatement_h
#define CaseStatement_h

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a CASE statement inside a SWITCH statement
   */
  class CaseStatement : public IStatement {
    IExpression* mExpression;
    Block* mBlock;
    bool mIsFallThrough;
    
    CaseStatement(IExpression* expression, Block* block, bool isFallThrough);
    
  public:
    
    ~CaseStatement();
    
    /**
     * Calculate the value of the case expression.
     * Exit with an error if the value is not ConstantInt
     */
    llvm::ConstantInt* getExpressionValue(IRGenerationContext& context) const;
    
    bool isFallThrough() const;
    
    void generateIR(IRGenerationContext& context) const override;
    
    /**
     * Create a CASE statement
     */
    static CaseStatement * newCaseStatement(IExpression* expression, Block* block);
    
    /**
     * Create a CASE statement with FALLTHROUGH
     */
    static CaseStatement * newCaseStatementWithFallThrough(IExpression* expression, Block* block);
    
  };
  
} /* namespace wisey */

#endif /* CaseStatement_hpp */

