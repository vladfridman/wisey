//
//  Long.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Long_h
#define Long_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a long constant
 */
class Long : public IExpression {

  long long mValue;
  
public:

  Long(long long value) : mValue(value) { }
  
  ~Long() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* Long_h */
