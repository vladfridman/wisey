//
//  ArrayAllocationStatic.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayAllocationStatic_h
#define ArrayAllocationStatic_h

#include "wisey/ArrayExactType.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents a static array allocation expression of the form: { 1, 2, 3, 4, 5 }
   */
  class ArrayAllocationStatic : public IExpression {
    
    ExpressionList mExpressionList;
    int mLine;
    
  public:
    
    ArrayAllocationStatic(ExpressionList expressionList, int line);
    
    ~ArrayAllocationStatic();

    /**
     * Returns list of expression used to initialize the array
     */
    ExpressionList getExpressionList() const;
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    const ArrayExactType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  private:
    
    const ArrayExactType* getExactType(IRGenerationContext& context) const;
    
    void checkArrayElements(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* ArrayAllocationStatic_h */
