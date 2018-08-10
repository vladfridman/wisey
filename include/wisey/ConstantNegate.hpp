//
//  ConstantNegate.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ConstantNegate_h
#define ConstantNegate_h

#include "wisey/IConstantExpression.hpp"

namespace wisey {
  
  /**
   * Negates a constant and generates a constant
   */
  class ConstantNegate : public IConstantExpression {
    const IConstantExpression* mExpression;
    int mLine;
    
  public:
    
    ConstantNegate(const IConstantExpression* expression, int line);
    
    ~ConstantNegate();
    
    int getLine() const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ConstantNegate_h */
