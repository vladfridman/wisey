//
//  FakeExpressionWithCleanup.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FakeExpressionWithCleanup_h
#define FakeExpressionWithCleanup_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * This is used to fake an expression that returns a given value and cleans up its type
   */
  class FakeExpressionWithCleanup : public IExpression {
    
    llvm::Value* mValue;
    const IType* mType;
    
  public:
    
    FakeExpressionWithCleanup(llvm::Value* value, const IType* type);
    
    ~FakeExpressionWithCleanup();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* FakeExpressionWithCleanup_h */

