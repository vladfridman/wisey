//
//  NegateExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NegateExpression_h
#define NegateExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * Represents negate operation
 */
class NegateExpression : public IExpression {
  IExpression* mExpression;
  
public:
  
  NegateExpression(IExpression* expression);
  
  ~NegateExpression();
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;

  llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};

} /* namespace wisey */

#endif /* NegateExpression_h */
