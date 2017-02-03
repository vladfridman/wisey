//
//  IntegerConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IntegerConstant_h
#define IntegerConstant_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents an integer constant
 */
class IntegerConstant : public IExpression {

  long mValue;
  
public:
  IntegerConstant(long value) : mValue(value) { }

  ~IntegerConstant() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* IntegerConstant_h */
