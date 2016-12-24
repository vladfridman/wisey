//
//  Float.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Float_h
#define Float_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

/**
 * Represents a float constant
 */
class Float: public IExpression {
  double mValue;
  
public:
  
  Float(double value) : mValue(value) { }

  ~Float() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

} /* namespace yazyk */

#endif /* Float_h */
