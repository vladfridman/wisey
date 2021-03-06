//
//  IncrementExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IncrementExpression_h
#define IncrementExpression_h

#include "IExpression.hpp"
#include "Identifier.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents an increment or decrement expression such as i++ or i--
   */
  class IncrementExpression : public IExpression {
    const IExpression* mExpression;
    long long mIncrementBy;
    bool mIsPrefix;
    int mLine;
    
    IncrementExpression(const IExpression* expression,
                        long long incrementBy,
                        bool isPrefix,
                        int line);
    
  public:
    
    ~IncrementExpression();

    /**
     * Create increment by one expression, e.g. i++
     */
    static IncrementExpression* newIncrementByOne(IExpression* expression, int line);

    /**
     * Create increment by one prefix expression, e.g. ++i
     */
    static IncrementExpression* newIncrementByOnePrefix(IExpression* expression, int line);

    /**
     * Create decrement by one expression
     */
    static IncrementExpression* newDecrementByOne(IExpression* expression, int line);

    /**
     * Create increment by one prefix expression, e.g. --i
     */
    static IncrementExpression* newDecrementByOnePrefix(IExpression* expression, int line);

    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void printIncrementToStream(IRGenerationContext& context, std::iostream& stream) const;
    
  };
  
} /* namespace wisey */

#endif /* IncrementExpression_h */

