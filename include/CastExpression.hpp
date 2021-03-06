//
//  CastExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CastExpression_h
#define CastExpression_h

#include "IExpression.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a cast operation
   */
  class CastExpression : public IExpression {
    const ITypeSpecifier* mTypeSpecifier;
    const IExpression* mExpression;
    int mLine;
    
  public:
    
    CastExpression(const ITypeSpecifier* typeSpecifier, const IExpression* expression, int line);
    
    ~CastExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* CastExpression_h */

