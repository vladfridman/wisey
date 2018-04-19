//
//  FakeExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FakeExpression_h
#define FakeExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * This is used to fake an expression that returns a given value on generateIR
   */
  class FakeExpression : public IExpression {
    
    llvm::Value* mValue;
    const IType* mType;
    
  public:
    
    FakeExpression(llvm::Value* value, const IType* type);
    
    ~FakeExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* FakeExpression_h */

