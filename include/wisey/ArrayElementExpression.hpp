//
//  ArrayElementExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayElementExpression_h
#define ArrayElementExpression_h

#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * Represents expression for referencing array element
 */
class ArrayElementExpression : public IExpression {
    
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
                                        std::vector<const IExpression*> arrayIndices);
  /**
   * Returns the pointer to the actual array from the array struct
   */
  static llvm::Value* unwrapArray(IRGenerationContext& context, llvm::Value* arrayPointer);

  llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;
  
  bool isConstant() const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  static void reportErrorArrayType(std::string typeName);
  
  static llvm::Value* getArrayElement(IRGenerationContext& context,
                                      llvm::Value* arrayPointer,
                                      const IExpression* indexExpression);
  
};

}

#endif /* ArrayElementExpression_h */
