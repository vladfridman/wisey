//
//  AdjustByExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef AdjustByExpression_h
#define AdjustByExpression_h

#include "wisey/IExpressionAssignable.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents an increment by or decrement by expression such as i += 1; or i -= 1;
   */
  class AdjustByExpression : public IExpressionAssignable {
    IExpression* mExpression;
    IExpression* mAdjustment;
    std::string mVariableName;
    bool mIsIncrement;
    int mLine;
    
    AdjustByExpression(IExpression* expression,
                        IExpression* adjustment,
                        std::string variableName,
                        bool isIncrement,
                        int line);
    
  public:
    
    ~AdjustByExpression();

    /**
     * Create increment by expression
     */
    static AdjustByExpression* newIncrementBy(IExpression* expression,
                                              IExpression* adjustment,
                                              int line);
    
    /**
     * Create decrement by expression
     */
    static AdjustByExpression* newDecrementBy(IExpression* expression,
                                              IExpression* adjustment,
                                              int line);

    int getLine() const override;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* AdjustByExpression_h */
