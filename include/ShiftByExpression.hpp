//
//  ShiftByExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ShiftByExpression_h
#define ShiftByExpression_h

#include "IBinaryExpression.hpp"
#include "Identifier.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a shift by expression such as a <<= b; or a >>= b;
   */
  class ShiftByExpression : public IBinaryExpression {
    const IExpression* mExpression;
    const IExpression* mAdjustment;
    bool mIsLeftShift;
    int mLine;
    
    ShiftByExpression(const IExpression* expression,
                      const IExpression* adjustment,
                      bool isLeftShift,
                      int line);

  public:
    
    ~ShiftByExpression();
    
    /**
     * Create shift left by expression
     */
    static ShiftByExpression* newShiftLeftBy(const IExpression* expression,
                                             const IExpression* adjustment,
                                             int line);

    /**
     * Create shift right by expression
     */
    static ShiftByExpression* newShiftRightBy(const IExpression* expression,
                                              const IExpression* adjustment,
                                              int line);
    
    const IExpression* getLeft() const override;
    
    const IExpression* getRight() const override;
    
    std::string getOperation() const override;

    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  };
  
} /* namespace wisey */

#endif /* ShiftByExpression_h */
