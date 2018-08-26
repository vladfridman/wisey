//
//  BitwiseOrExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef BitwiseOrExpression_h
#define BitwiseOrExpression_h

#include "wisey/IBinaryExpression.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents bitwise OR operation
   */
  class BitwiseOrExpression : public IBinaryExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    int mLine;
    
  public:
    
    BitwiseOrExpression(const IExpression* left, const IExpression* right, int line);
    
    ~BitwiseOrExpression();
    
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

#endif /* BitwiseOrExpression_h */
