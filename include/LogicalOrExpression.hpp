//
//  LogicalOrExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalOrExpression_h
#define LogicalOrExpression_h

#include "IExpression.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a logical OR expression such as 'a || b'
   */
  class LogicalOrExpression : public IExpression {
    const IExpression* mLeftExpression;
    const IExpression* mRightExpression;
    int mLine;
    
  public:
    
    LogicalOrExpression(const IExpression* leftExpression,
                        const IExpression* rightExpression,
                        int line);
    
    ~LogicalOrExpression();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LogicalOrExpression_h */

