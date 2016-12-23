//
//  Char.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Char_h
#define Char_h

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents a constant character
 */
class Char : public IExpression {
  char mValue;
  
public:

  Char(char value) : mValue(value) { }
  
  ~Char() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
} /* namespace yazyk */

#endif /* Char_h */
