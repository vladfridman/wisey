//
//  NullExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NullExpression_h
#define NullExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * Represents the null
 */
class NullExpression : public IExpression {
  
public:
  
  NullExpression() { }
  
  ~NullExpression() { }
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Constant* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};

}

#endif /* NullExpression_h */
