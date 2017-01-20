//
//  Boolean.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Boolean_h
#define Boolean_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a constant boolean
 */
class Boolean : public IExpression {
  bool mValue;
  
public:
  
  Boolean(bool value) : mValue(value) { }
  
  ~Boolean() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
static Boolean True(true);

static Boolean False(false);
  
} /* namespace yazyk */

#endif /* Boolean_h */
