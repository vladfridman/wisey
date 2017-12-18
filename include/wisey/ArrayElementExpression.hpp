//
//  ArrayElementExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayElementExpression_h
#define ArrayElementExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * Represents expression for referencing array element
 */
class ArrayElementExpression : public IExpression {
    
  const IExpression* mArrayExpression;
  const IExpression* mArrayIndexExpresion;
  
public:
  
  ArrayElementExpression(const IExpression* arrayExpression,
                         const IExpression* arrayIndexExpresion);
  
  ~ArrayElementExpression();
  
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;
  
  bool isConstant() const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  void reportErrorArrayType(std::string typeName) const;
  
};

}

#endif /* ArrayElementExpression_h */
