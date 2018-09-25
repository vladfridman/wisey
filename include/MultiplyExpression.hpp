//
//  MultiplyExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef MultiplyExpression_h
#define MultiplyExpression_h

#include "IArithmeticExpression.hpp"

namespace wisey {
  
  /**
   * Represents a multiply expression such as 'a * b'
   */
  class MultiplyExpression : public IArithmeticExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    int mLine;
    
  public:
    
    MultiplyExpression(const IExpression* left, const IExpression* right, int line);
    
    ~MultiplyExpression();
    
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

#endif /* MultiplyExpression_h */
