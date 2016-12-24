//
//  Integer.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Integer_h
#define Integer_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents an integer constant
 */
class Integer : public IExpression {

  long mValue;
  
public:
  Integer(long value) : mValue(value) { }

  ~Integer() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

} /* namespace yazyk */

#endif /* Integer_h */
