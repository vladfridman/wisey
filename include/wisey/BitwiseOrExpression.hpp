//
//  BitwiseOrExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef BitwiseOrExpression_h
#define BitwiseOrExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents bitwise OR operation
   */
  class BitwiseOrExpression : public IExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    int mLine;
    
  public:
    
    BitwiseOrExpression(const IExpression* left, const IExpression* right, int line);
    
    ~BitwiseOrExpression();
    
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

#endif /* BitwiseOrExpression_h */
