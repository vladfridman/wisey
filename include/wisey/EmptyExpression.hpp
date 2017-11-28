//
//  EmptyExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyExpression_h
#define EmptyExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
/**
 * Represents an empty expression that does not do anything
 */
class EmptyExpression : public IExpression {

public:

  const static EmptyExpression EMPTY_EXPRESSION;

  EmptyExpression();
  
  ~EmptyExpression();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
 
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* EmptyExpression_h */
