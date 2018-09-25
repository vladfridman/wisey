//
//  NegateExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NegateExpression_h
#define NegateExpression_h

#include "IExpression.hpp"
#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents negate operation
   */
  class NegateExpression : public IExpression {
    const IExpression* mExpression;
    int mLine;
    
  public:
    
    NegateExpression(const IExpression* expression, int line);
    
    ~NegateExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* NegateExpression_h */

