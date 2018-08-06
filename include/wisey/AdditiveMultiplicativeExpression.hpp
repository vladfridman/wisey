//
//  AdditiveMultiplicativeExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef AdditiveMultiplicativeExpression_hpp
#define AdditiveMultiplicativeExpression_hpp

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents simple binary expression such as addition or division
   */
  class AdditiveMultiplicativeExpression : public IExpression {
    const IExpression* mLeftExpression;
    const IExpression* mRightExpression;
    int mOperation;
    int mLine;
    
  public:
    
    AdditiveMultiplicativeExpression(const IExpression* leftExpression,
                                     int operation,
                                     const IExpression* rightExpression,
                                     int line);
    
    ~AdditiveMultiplicativeExpression();
    
    /**
     * Returns the left side of the expression
     */
    const IExpression* getLeft() const;
    
    /**
     * Returns the right side of the expression
     */
    const IExpression* getRight() const;
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void checkTypes(IRGenerationContext& context,
                    const IType* leftType,
                    const IType* rightType) const;
  
    bool isPointerArithmetic(const IType* leftType, const IType* rightType) const;
    
  };
  
} /* namespace wisey */

#endif /* AdditiveMultiplicativeExpression_hpp */

