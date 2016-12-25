//
//  Double.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Double_h
#define Double_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a double constant
 */
class Double : public IExpression {
  long double mValue;
  
public:

  Double(long double value) : mValue(value) { }
  
  ~Double() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* Double_h */
