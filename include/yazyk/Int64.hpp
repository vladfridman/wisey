//
//  Int64.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Int64_h
#define Int64_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a 64 bit integer constant
 */
class Int64 : public IExpression {

  long long mValue;
  
public:

  Int64(long long value) : mValue(value) { }
  
  ~Int64() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* Int64_hpp */
