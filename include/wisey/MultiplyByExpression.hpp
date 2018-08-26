//
//  MultiplyByExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/26/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef MultiplyByExpression_h
#define MultiplyByExpression_h

#include "wisey/IBinaryExpression.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a *= b operation
   */
  class MultiplyByExpression : public IBinaryExpression {
    const IExpression* mExpression;
    const IExpression* mAdjustment;
    int mLine;
    
  public:
    
    MultiplyByExpression(const IExpression* expression, const IExpression* adjustment, int line);
    
    ~MultiplyByExpression();
    
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

#endif /* MultiplyByExpression_h */
