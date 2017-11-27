//
//  LongConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LongConstant_h
#define LongConstant_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {

/**
 * Represents a long constant
 */
class LongConstant : public IExpression {

  long long mValue;
  
public:

  LongConstant(long long value) : mValue(value) { }
  
  ~LongConstant() {}
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Constant* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* LongConstant_h */
