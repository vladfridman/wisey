//
//  Int32.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Int32_h
#define Int32_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a 32 bit integer constant
 */
class Int32 : public IExpression {

  long mValue;
  
public:
  Int32(long value) : mValue(value) { }

  ~Int32() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* Int32_h */
