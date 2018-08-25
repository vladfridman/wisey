//
//  BooleanNotExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanNotExpression_h
#define BooleanNotExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a boolean NOT operation
   */
  class BooleanNotExpression : public IExpression {
    const IExpression* mExpression;
    int mLine;
    
  public:
    
    BooleanNotExpression(const IExpression* expression, int line);
    
    ~BooleanNotExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* BooleanNotExpression_h */

