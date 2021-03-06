//
//  ConditionalExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ConditionalExpression_h
#define ConditionalExpression_h

#include "IExpression.hpp"

namespace wisey {
  
  /**
   * Represents a condition expression such as a == b ? 1 : 2
   *
   * When conditional expression is of owner type both of expressions are realeased and the one that
   * did not get returned as result is freed.
   */
  class ConditionalExpression : public IExpression {
    
    const IExpression* mConditionExpression;
    const IExpression* mIfTrueExpression;
    const IExpression* mIfFalseExpression;
    int mLine;
    
  public:
    
    ConditionalExpression(const IExpression* conditionExpression,
                          const IExpression* ifTrueExpression,
                          const IExpression* ifFalseExpression,
                          int line);
    
    ~ConditionalExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void checkTypes(IRGenerationContext& context) const;
  };
  
} /* namespace wisey */

#endif /* ConditionalExpression_h */

