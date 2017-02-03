//
//  DoubleConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef DoubleConstant_h
#define DoubleConstant_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a double constant
 */
class DoubleConstant : public IExpression {
  long double mValue;
  
public:

  DoubleConstant(long double value) : mValue(value) { }
  
  ~DoubleConstant() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* DoubleConstant_h */
