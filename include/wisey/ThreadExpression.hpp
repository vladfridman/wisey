//
//  ThreadExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadExpression_h
#define ThreadExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents the current thread in a wisey program.
 */
class ThreadExpression : public IExpression {
  
public:
  
  ThreadExpression() { }
  
  ~ThreadExpression() { }
  
  static std::string THREAD;
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* ThreadExpression_h */
