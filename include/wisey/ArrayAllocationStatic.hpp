//
//  ArrayAllocationStatic.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayAllocationStatic_h
#define ArrayAllocationStatic_h

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
    
    ExpressionList getExpressionList() const;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    bool isConstant() const override;
    
    const ArrayOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void checkArrayElements(IRGenerationContext& context) const;
    
    void initializeArray(IRGenerationContext& context,
                         llvm::Value* arrayStructPointer,
                         const ArrayType* arrayType) const;
    
    ExpressionList flattenExpressionList(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* ArrayAllocationStatic_h */
