//
//  AdjustByExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustByExpression_h
#define AdjustByExpression_h

#include "IBinaryExpression.hpp"
#include "Identifier.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents an increment by or decrement by expression such as i += 1; or i -= 1;
   */
  class AdjustByExpression : public IBinaryExpression {
    const IExpression* mExpression;
    const IExpression* mAdjustment;
    std::string mVariableName;
    bool mIsIncrement;
    int mLine;
    
    AdjustByExpression(const IExpression* expression,
                       const IExpression* adjustment,
                       std::string variableName,
                       bool isIncrement,
                       int line);

  public:
    
    ~AdjustByExpression();

    /**
     * Create increment by expression
     */
    static AdjustByExpression* newIncrementBy(const IExpression* expression,
                                              const IExpression* adjustment,
                                              int line);
    
    /**
     * Create decrement by expression
     */
    static AdjustByExpression* newDecrementBy(const IExpression* expression,
                                              const IExpression* adjustment,
                                              int line);
    
    const IExpression* getLeft() const override;
    
    const IExpression* getRight() const override;
    
    std::string getOperation() const override;

    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* AdjustByExpression_h */
