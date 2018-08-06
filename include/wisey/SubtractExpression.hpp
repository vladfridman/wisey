//
//  SubtractExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef SubtractExpression_h
#define SubtractExpression_h

#include "wisey/IArithmeticExpression.hpp"

namespace wisey {
  
  /**
   * Represents a subtract expression such as 'a - b'
   */
  class SubtractExpression : public IArithmeticExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    int mLine;
    
  public:
    
    SubtractExpression(const IExpression* left, const IExpression* right, int line);
    
    ~SubtractExpression();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* subtractTwoPointers(IRGenerationContext& context,
                                     const IType* leftType,
                                     llvm::Value* leftValue,
                                     const IType* rightType,
                                     llvm::Value* rightValue) const;
    
    llvm::Value* subtractIntFromPointer(IRGenerationContext& context,
                                        const IType* leftType,
                                        llvm::Value* leftValue,
                                        const IType* rightType,
                                        llvm::Value* rightValue) const;

  };
  
} /* namespace wisey */

#endif /* SubtractExpression_h */
