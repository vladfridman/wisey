//
//  ReminderByExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReminderByExpression_h
#define ReminderByExpression_h

#include "IBinaryExpression.hpp"
#include "Identifier.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a %= b operation
   */
  class ReminderByExpression : public IBinaryExpression {
    const IExpression* mExpression;
    const IExpression* mAdjustment;
    int mLine;
    
  public:
    
    ReminderByExpression(const IExpression* expression, const IExpression* adjustment, int line);
    
    ~ReminderByExpression();
    
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

#endif /* ReminderByExpression_h */
