//
//  LogicalAndExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalAndExpression_h
#define LogicalAndExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a logical AND expression such as 'a && b'
   */
  class LogicalAndExpression : public IExpression {
    const IExpression* mLeftExpression;
    const IExpression* mRightExpression;
    int mLine;
    
  public:
    
    LogicalAndExpression(const IExpression* leftExpression,
                         const IExpression* rightExpression,
                         int line);
    
    ~LogicalAndExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LogicalAndExpression_h */

