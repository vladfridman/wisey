//
//  Float32.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Float32_h
#define Float32_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a 32 bit float constant
 */
class Float32: public IExpression {
  double mValue;
  
public:
  
  Float32(double value) : mValue(value) { }

  ~Float32() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* Float32_h */
