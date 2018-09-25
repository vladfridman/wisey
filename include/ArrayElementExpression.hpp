//
//  ArrayElementExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayElementExpression_h
#define ArrayElementExpression_h

#include "ArrayType.hpp"
#include "IExpressionAssignable.hpp"

namespace wisey {
  
  /**
   * Represents expression for referencing array element
   */
  class ArrayElementExpression : public IExpressionAssignable {
    
    const IExpression* mArrayExpression;
    const IExpression* mArrayIndexExpresion;
    int mLine;
    
  public:
    
    ArrayElementExpression(const IExpression* arrayExpression,
                           const IExpression* arrayIndexExpresion,
                           int line);
    
    ~ArrayElementExpression();
    
    /**
     * Given list of indices and array pointer returns pointer to the array element
     */
    static llvm::Value* generateElementIR(IRGenerationContext& context,
                                          const ArrayType* arrayType,
                                          llvm::Value* arrayStructPointer,
                                          std::vector<const IExpression*> arrayIndices,
                                          int line);
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void reportErrorArrayType(IRGenerationContext& context, std::string typeName) const;
    
    static llvm::Value* getArrayElement(IRGenerationContext& context,
                                        llvm::Value* arrayStructPointer,
                                        const IExpression* indexExpression,
                                        int line);
    
  };
  
}

#endif /* ArrayElementExpression_h */

