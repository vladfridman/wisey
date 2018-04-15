//
//  LogicalOrExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalOrExpression_h
#define LogicalOrExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a logical OR expression such as 'a || b'
   */
  class LogicalOrExpression : public IExpression {
    IExpression* mLeftExpression;
    IExpression* mRightExpression;
    int mLine;
    
  public:
    
    LogicalOrExpression(IExpression* leftExpression, IExpression* rightExpression, int line);
    
    ~LogicalOrExpression();
    
    int getLine() const override;

    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LogicalOrExpression_h */

