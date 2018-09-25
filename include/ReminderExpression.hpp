//
//  ReminderExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReminderExpression_h
#define ReminderExpression_h

#include "IArithmeticExpression.hpp"

namespace wisey {
  
  /**
   * Represents a reminder expression such as 'a % b'
   */
  class ReminderExpression : public IArithmeticExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    int mLine;
    
  public:
    
    ReminderExpression(const IExpression* left, const IExpression* right, int line);
    
    ~ReminderExpression();
    
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

#endif /* ReminderExpression_h */
