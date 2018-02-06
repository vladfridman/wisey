//
//  ArrayAllocationStatic.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayAllocationStatic_h
#define ArrayAllocationStatic_h

#include "wisey/ArrayExactOwnerType.hpp"
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
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    bool isConstant() const override;
    
    const ArrayExactOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  private:
    
    void initializeArray(IRGenerationContext& context,
                         llvm::Value* arrayStructPointer,
                         const IType* elementType,
                         std::list<unsigned long> dimensions) const;

    const ArrayExactType* getExactType(IRGenerationContext& context) const;
    
    void checkArrayElements(IRGenerationContext& context) const;
    
    ExpressionList flattenExpressionList(IRGenerationContext& context) const;
    
    static llvm::Value* allocateArray(IRGenerationContext &context,
                                      const ArrayExactType* arrayType);

    static void initializeEmptyArray(IRGenerationContext& context,
                                     llvm::Value* arrayStructPointer,
                                     std::list<unsigned long> dimensions);

  };
  
} /* namespace wisey */

#endif /* ArrayAllocationStatic_h */
