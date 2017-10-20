//
//  AdditiveMultiplicativeExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef AdditiveMultiplicativeExpression_hpp
#define AdditiveMultiplicativeExpression_hpp

#include "wisey/IExpression.hpp"

namespace wisey {

/**
 * Represents simple binary expression such as addition or division
 */
class AdditiveMultiplicativeExpression : public IExpression {
  IExpression* mLeftExpression;
  IExpression* mRightExpression;
  int mOperation;
  
public:

  AdditiveMultiplicativeExpression(IExpression* leftExpression,
                                   int operation,
                                   IExpression* rightExpression) :
    mLeftExpression(leftExpression), mRightExpression(rightExpression), mOperation(operation) { }
  
  ~AdditiveMultiplicativeExpression();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  void checkTypes(const IType* leftType, const IType* rightType) const;
};

} /* namespace wisey */

#endif /* AdditiveMultiplicativeExpression_hpp */
