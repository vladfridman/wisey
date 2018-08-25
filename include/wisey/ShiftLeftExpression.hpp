//
//  ShiftLeftExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ShiftLeftExpression_h
#define ShiftLeftExpression_h

#include "wisey/IBinaryExpression.hpp"

namespace wisey {
  
  /**
   * Represents shift left '<<' wisey operation
   */
  class ShiftLeftExpression : public IBinaryExpression {
    const IExpression* mLeftExpression;
    const IExpression* mRightExpression;
    int mLine;
    
  public:
    
    ShiftLeftExpression(const IExpression* leftExpression,
                        const IExpression* rightExpression,
                        int line);

    ~ShiftLeftExpression();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    const IExpression* getLeft() const override;
    
    const IExpression* getRight() const override;
    
    std::string getOperation() const override;
    
  };
  
} /* namespace wisey */

#endif /* ShiftLeftExpression_h */
