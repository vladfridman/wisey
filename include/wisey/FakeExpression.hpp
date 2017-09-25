//
//  FakeExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FakeExpression_h
#define FakeExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * This is used to fake an expression that returns a given value on generateIR
 */
class FakeExpression : public IExpression {

  llvm::Value* mValue;
  const IType* mType;
  
public:
  
  FakeExpression(llvm::Value* value, const IType* type) : mValue(value), mType(type) { }
  
  ~FakeExpression() { }
  
  IVariable* getVariable(IRGenerationContext& context) const override;
 
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* FakeExpression_h */
