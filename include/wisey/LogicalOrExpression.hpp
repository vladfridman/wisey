//
//  LogicalOrExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LogicalOrExpression_h
#define LogicalOrExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {

/**
 * Represents a logical OR expression such as 'a || b'
 */
class LogicalOrExpression : public IExpression {
  IExpression* mLeftExpression;
  IExpression* mRightExpression;
  
public:
  LogicalOrExpression(IExpression* leftExpression, IExpression* rightExpression) :
    mLeftExpression(leftExpression),
    mRightExpression(rightExpression) { }
  
  ~LogicalOrExpression();
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};

} /* namespace wisey */

#endif /* LogicalOrExpression_h */
