//
//  Float64.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Float64_h
#define Float64_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a double constant
 */
class Float64 : public IExpression {
  long double mValue;
  
public:

  Float64(long double value) : mValue(value) { }
  
  ~Float64() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* Float64_h */
