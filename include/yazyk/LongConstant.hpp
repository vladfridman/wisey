//
//  LongConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LongConstant_h
#define LongConstant_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a long constant
 */
class LongConstant : public IExpression {

  long long mValue;
  
public:

  LongConstant(long long value) : mValue(value) { }
  
  ~LongConstant() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* LongConstant_h */
