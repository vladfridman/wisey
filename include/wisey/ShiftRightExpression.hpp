//
//  ShiftRightExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ShiftRightExpression_h
#define ShiftRightExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents shift right '>>' wisey operation
   */
  class ShiftRightExpression : public IExpression {
    const IExpression* mLeftExpression;
    const IExpression* mRightExpression;
    int mLine;
    
  public:
    
    ShiftRightExpression(const IExpression* leftExpression,
                         const IExpression* rightExpression,
                         int line);
    
    ~ShiftRightExpression();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void checkTypes(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* ShiftRightExpression_h */
