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
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* LongConstant_h */
